/*
 * Copyright (C) 2014 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Ken VanDine <ken.vandine@canonical.com>
 *
 */

import GSettings 1.0
import QtQuick 2.0
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import SystemSettings 1.0
import MeeGo.QOfono 0.2
import "sims.js" as Sims


ItemPage {
    id: root
    title: i18n.tr("SIM PIN")

    property var sims
    property var curSim

    Component {
        id: dialogComponent

        Dialog {
            id: changePinDialog
            title: i18n.tr("Change SIM PIN")

            property string errorText: i18n.tr(
                    "Incorrect PIN. %1 attempt remaining.",
                    "Incorrect PIN. %1 attempts remaining.",
                    curSim.pinRetries[OfonoSimManager.SimPin] || 3
                    ).arg(curSim.pinRetries[OfonoSimManager.SimPin] || 3)
            property int simMin: curSim.minimumPinLength(OfonoSimManager.SimPin)
            property int simMax: curSim.maximumPinLength(OfonoSimManager.SimPin)

            // This is a bit hacky, but the contents of this dialog get so tall
            // that on a mako device, they don't fit with the OSK also visible.
            // So we scrunch up spacing.
            Binding {
                target: __foreground
                property: "itemSpacing"
                value: units.gu(1)
            }

            Connections {
                target: curSim
                onChangePinComplete: {
                    if (error === OfonoSimManager.FailedError) {
                        console.warn("Change PIN failed with: " + error);
                        incorrect.visible = true;
                        changePinDialog.enabled = true;
                        currentInput.forceActiveFocus();
                        currentInput.selectAll();
                        return;
                    }
                    incorrect.visible = false;
                    changePinDialog.enabled = true;
                    PopupUtils.close(changePinDialog);
                }
            }

            Label {
                text: i18n.tr("Current PIN:")
            }

            TextField {
                id: currentInput
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhDialableCharactersOnly
                maximumLength: simMax
            }

            Label {
                id: retries
                text: i18n.tr("%1 attempt allowed.", "%1 attempts allowed.",
                              curSim.pinRetries[OfonoSimManager.SimPin] || 3).arg(
                          curSim.pinRetries[OfonoSimManager.SimPin] || 3)
                visible: !incorrect.visible
            }

            Label {
                id: incorrect
                text: errorText
                visible: false
                color: "darkred"
            }

            Label {
                text: i18n.tr("Choose new PIN:")
            }

            TextField {
                id: newInput
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhDialableCharactersOnly
                maximumLength: simMax
            }

            Label {
                text: i18n.tr("Confirm new PIN:")
            }

            TextField {
                id: confirmInput
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhDialableCharactersOnly
                maximumLength: simMax

                // Doesn't get updated if you set this in enabled of confirmButton
                onTextChanged: confirmButton.enabled =
                               (acceptableInput &&
                                text.length >= simMin &&
                                (text === newInput.text))
            }

            Label {
                id: notMatching
                wrapMode: Text.Wrap
                text: i18n.tr("PINs don't match. Try again.")
                visible: (newInput.length === confirmInput.length) &&
                         (newInput.text != confirmInput.text)
                color: "darkred"
            }

            RowLayout {
                spacing: units.gu(1)

                Button {
                    Layout.fillWidth: true
                    color: UbuntuColors.lightGrey
                    text: i18n.tr("Cancel")
                    onClicked: PopupUtils.close(changePinDialog)
                }

                Button {
                    id: confirmButton
                    Layout.fillWidth: true
                    color: UbuntuColors.green
                    text: i18n.tr("Change")
                    enabled: false
                    onClicked: {
                        changePinDialog.enabled = false
                        var match = (newInput.text === confirmInput.text)
                        notMatching.visible = !match
                        if (!match) {
                            changePinDialog.enabled = true
                            newInput.forceActiveFocus()
                            newInput.selectAll()
                            return
                        }
                        curSim.changePin(OfonoSimManager.SimPin, currentInput.text, newInput.text);
                    }
                }
            }
        }
    }

    Component {
        id: lockDialogComponent

        Dialog {
            id: lockPinDialog
            objectName: "lockDialogComponent"
            title: curSim.lockedPins.length > 0 ?
                       i18n.tr("Enter SIM PIN") :
                       i18n.tr("Enter Previous SIM PIN")

            property string errorText: i18n.tr(
                    "Incorrect PIN. %1 attempts remaining."
                    ).arg(curSim.pinRetries[OfonoSimManager.SimPin] || 3)

            property int simMin: curSim.minimumPinLength(OfonoSimManager.SimPin)
            property int simMax: curSim.maximumPinLength(OfonoSimManager.SimPin)

            // This is a bit hacky, but the contents of this dialog get so tall
            // that on a mako device, they don't fit with the OSK also visible.
            // So we scrunch up spacing.
            Binding {
                target: __foreground
                property: "itemSpacing"
                value: units.gu(1)
            }

            Connections {
                target: curSim
                onLockPinComplete: {
                    if (error === OfonoSimManager.FailedError) {
                        console.warn("Lock PIN failed with: " + error);
                        incorrect.visible = true;
                        lockPinDialog.enabled = true;
                        prevInput.forceActiveFocus();
                        prevInput.selectAll();
                        return;
                    }
                    incorrect.visible = false;
                    lockPinDialog.enabled = true;
                    PopupUtils.close(lockPinDialog);
                }
                onUnlockPinComplete: {
                    if (error === OfonoSimManager.FailedError) {
                        console.warn("Unlock PIN failed with: " + error);
                        incorrect.visible = true;
                        lockPinDialog.enabled = true;
                        prevInput.forceActiveFocus();
                        prevInput.selectAll();
                        return;
                    }
                    incorrect.visible = false;
                    lockPinDialog.enabled = true;
                    PopupUtils.close(lockPinDialog);
                }
            }

            TextField {
                id: prevInput
                objectName: "prevInput"
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhDialableCharactersOnly
                maximumLength: simMax

                // Doesn't get updated if you set this in enabled of confirmButton
                onTextChanged: lockButton.enabled =
                               (acceptableInput && text.length >= simMin)
            }

            Label {
                horizontalAlignment: Text.AlignHCenter
                text: i18n.tr("%1 attempts allowed.").arg(
                          curSim.pinRetries[OfonoSimManager.SimPin] || 3)
                visible: !incorrect.visible
                width: parent.width
            }

            Label {
                id: incorrect
                text: errorText
                visible: false
                color: "darkred"
            }

            RowLayout {
                spacing: units.gu(1)

                Button {
                    objectName: "cancelButton"
                    Layout.fillWidth: true
                    color: UbuntuColors.lightGrey
                    text: i18n.tr("Cancel")
                    onClicked: {
                        if (curSim.lockedPins.length < 1)
                            caller.checked = false;
                        else
                            caller.checked = true;
                        PopupUtils.close(lockPinDialog);
                    }
                }

                Button {
                    id: lockButton
                    objectName: "lockButton"
                    Layout.fillWidth: true
                    color: UbuntuColors.green

                    text: curSim.lockedPins.length > 0 ? i18n.tr("Unlock") : i18n.tr("Lock")
                    enabled: false
                    onClicked: {
                        lockPinDialog.enabled = false;
                        if (curSim.lockedPins.length > 0)
                            curSim.unlockPin(OfonoSimManager.SimPin, prevInput.text);
                        else
                            curSim.lockPin(OfonoSimManager.SimPin, prevInput.text);
                    }
                }
            }
        }
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        Repeater {
            model: sims.length
            Column {
                id: sim
                anchors {
                    left: parent.left
                    right: parent.right
                }

                states: [
                    State {
                        name: "locked"
                        when: sims[index].simMng.pinRequired !== OfonoSimManager.NoPin
                    },
                    State {
                        name: "unlocked"
                        when: sims[index].simMng.pinRequired === OfonoSimManager.NoPin
                    }
                ]

                Connections {
                    target: sims[index].simMng
                    onLockedPinsChanged: {
                        simPinSwitch.checked =
                                sims[index].simMng.lockedPins.length > 0;
                    }
                }

                ListItem.Standard {
                    text: sims[index].title
                    visible: sims.length > 1
                }

                ListItem.Standard {
                    text: i18n.tr("SIM PIN")
                    control: Switch {
                        id: simPinSwitch
                        objectName: "simPinSwitch"
                        checked: sims[index].simMng.lockedPins.length > 0
                        onClicked: {
                            curSim = sims[index].simMng;
                            PopupUtils.open(lockDialogComponent, simPinSwitch);
                        }
                    }
                }

                ListItem.Standard {
                    id: changeControl
                    visible: sim.state === "unlocked"
                    text: i18n.tr("Unlocked")
                    control: Button {
                        enabled: parent.visible
                        text: i18n.tr("Change PIN…")
                        onClicked: {
                            curSim = sims[index].simMng;
                            PopupUtils.open(dialogComponent);
                        }
                    }
                }
                ListItem.Standard {
                    id: lockControl
                    visible: sim.state === "locked"
                    text: i18n.tr("Locked")
                    control: Button {
                        enabled: sims[index].simMng.pinRequired !== 'none'
                        text: i18n.tr("Unlock…")
                        color: UbuntuColors.green
                        onClicked: {
                            curSim = sims[index].simMng;
                            PopupUtils.open(dialogComponent);
                        }
                    }
                }
                ListItem.Divider {
                    visible: index < (sims.length - 1)
                }

            }
        }

        ListItem.Caption {
            text: i18n.tr("When a SIM PIN is set, it must be entered to access cellular services after restarting the phone or swapping the SIM.")
        }

        ListItem.Caption {
            text: i18n.tr("Entering an incorrect PIN repeatedly may lock the SIM permanently.")
        }
    }
}
