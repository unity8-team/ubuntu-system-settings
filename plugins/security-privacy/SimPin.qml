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


ItemPage {
    id: page
    title: i18n.tr("SIM PIN")

    OfonoManager {
        id: manager
    }

    OfonoSimManager {
        id: sim
        modemPath: manager.modems[0]
    }

    property string errorText: i18n.tr("Incorrect PIN. %1 attempts remaining.").arg(sim.pinRetries[OfonoSimManager.SimPin])
    property int simMin: sim.minimumPinLength(OfonoSimManager.SimPin)
    property int simMax: sim.maximumPinLength(OfonoSimManager.SimPin)

    Component {
        id: dialogComponent

        Dialog {
            id: changePinDialog
            title: i18n.tr("Change SIM PIN")

            // This is a bit hacky, but the contents of this dialog get so tall
            // that on a mako device, they don't fit with the OSK also visible.
            // So we scrunch up spacing.
            Binding {
                target: __foreground
                property: "itemSpacing"
                value: units.gu(1)
            }

            Connections {
                target: sim
                onChangePinComplete: {
                    console.warn("onChangePinComplete: " + error);
                    if (error !== OfonoSimManager.NoError) {
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
                text: i18n.tr("%1 attempts remaining").arg(sim.pinRetries[OfonoSimManager.SimPin])
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
                visible: false
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
                        sim.changePin(OfonoSimManager.SimPin, currentInput.text, newInput.text);
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
            title: i18n.tr("Enter SIM PIN")

            // This is a bit hacky, but the contents of this dialog get so tall
            // that on a mako device, they don't fit with the OSK also visible.
            // So we scrunch up spacing.
            Binding {
                target: __foreground
                property: "itemSpacing"
                value: units.gu(1)
            }

            Connections {
                target: sim
                onLockPinComplete: {
                    console.warn("onLockPinComplete: " + error);
                    if (error !== OfonoSimManager.NoError) {
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
                    console.warn("onUnlockPinComplete: " + error);
                    if (error !== OfonoSimManager.NoError) {
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
                text: i18n.tr("%1 attempts remaining").arg(sim.pinRetries[OfonoSimManager.SimPin])
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
                    onClicked: PopupUtils.close(lockPinDialog)
                }

                Button {
                    id: lockButton
                    objectName: "lockButton"
                    Layout.fillWidth: true
                    color: UbuntuColors.green

                    text: sim.lockedPins.length > 0 ? i18n.tr("Unlock") : i18n.tr("Lock")
                    enabled: false
                    onClicked: {
                        lockPinDialog.enabled = false;
                        if (sim.lockedPins.length > 0)
                            sim.unlockPin(OfonoSimManager.SimPin, prevInput.text);
                        else
                            sim.lockPin(OfonoSimManager.SimPin, prevInput.text);
                    }
                }
            }
        }
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.Standard {
            text: i18n.tr("SIM PIN")
            control: Switch {
                id: simPinSwitch
                objectName: "simPinSwitch"
                checked: sim.lockedPins.length > 0
                onClicked: {
                    PopupUtils.open(lockDialogComponent);
                }
            }
        }

        ListItem.SingleControl {
            id: changeControl
            visible: sim.lockedPins.length > 0
            control: Button {
                enabled: parent.visible
                text: i18n.tr("Change PIN…")
                width: parent.width - units.gu(4)
                onClicked: PopupUtils.open(dialogComponent)
            }
            showDivider: false
        }

        ListItem.Caption {
            text: i18n.tr("When a SIM PIN is set, it must be entered to access cellular services after restarting the phone or swapping the SIM.  Entering an incorrect PIN repeatedly may lock the SIM permanently.")
        }
    }
}
