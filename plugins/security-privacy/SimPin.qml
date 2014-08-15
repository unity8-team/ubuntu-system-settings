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
        onChangePinComplete: {
            console.warn("onChangePinComplete: " + errorString);
            errorText = errorString;
            if (error !== OfonoSimManager.NoError) {
                changePinDialog.enabled = true
                currentInput.forceActiveFocus()
                currentInput.selectAll()
                return
            }

            changePinDialog.enabled = true
            PopupUtils.close(changePinDialog)
        }
    }

    property string errorText: ""

    Component {
        id: dialogComponent

        Dialog {
            id: changePinDialog

            // This is a bit hacky, but the contents of this dialog get so tall
            // that on a mako device, they don't fit with the OSK also visible.
            // So we scrunch up spacing.
            Binding {
                target: __foreground
                property: "itemSpacing"
                value: units.gu(1)
            }

            title: i18n.tr("Change SIM PIN")

            Label {
                text: i18n.tr("Current PIN:")
                visible: currentInput.visible
            }

            TextField {
                id: currentInput
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhDialableCharactersOnly

                inputMask: "00000000"
            }

            Label {
                id: incorrect
                text: errorText
                visible: text !== ""
                color: "darkred"
            }

            Label {
                text: i18n.tr("Choose new PIN:")
                visible: newInput.visible
            }

            TextField {
                id: newInput
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhDialableCharactersOnly
                inputMask: "00000000"

                // Doesn't get updated if you set this in enabled of confirmButton
                onTextChanged: confirmButton.enabled =
                               (acceptableInput && (!visible || text.length > 0))
            }

            Label {
                text: i18n.tr("Confirm new PIN:")
                visible: confirmInput.visible
            }

            TextField {
                id: confirmInput
                echoMode: TextInput.Password
                inputMethodHints: Qt.ImhDialableCharactersOnly
                inputMask: "00000000"
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

                        console.warn("lockedPins: " + sim.lockedPins);
                        sim.changePin(OfonoSimManager.SimPin, currentInput.visible ? currentInput.text : "", newInput.text);
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
                checked: false //sim.pinRequired !== PinType.NoPin
            }
        }


        ListItem.SingleControl {

            id: changeControl
            visible: simPinSwitch.checked

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
