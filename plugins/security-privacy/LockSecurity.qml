/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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
 */

import GSettings 1.0
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import SystemSettings 1.0

ItemPage {
    title: i18n.tr("Lock security")

    GSettings {
        id: settingsSchema
        schema.id: "com.ubuntu.touch.system-settings"
    }

    function getUnlockMethod() {
        switch (settingsSchema.unlockMethod) {
            case "swipe":
                return 0
            case "passcode":
                return 1
            case "password":
                return 2
        }
    }

    function indexToMethod(index) {
        switch (index) {
            case 0:
                return "swipe"
            case 1:
                return "passcode"
            case 2:
                return "password"
        }
    }

    Dialog {
        id: changeSecurityDialog

        property string oldMethod: settingsSchema.unlockMethod
        property string newMethod: indexToMethod(unlockMethod.selectedIndex)

        function clearInputs() {
            currentInput.text = ""
            newInput.text = ""
            confirmInput.text = ""
        }

        title: {
            switch (changeSecurityDialog.newMethod) {
            case "swipe":
                return i18n.tr("Switch to Swipe")
            case "passcode":
                return i18n.tr("Switch to Passcode")
            case "password":
                return i18n.tr("Switch to Passphrase")
            }
        }

        Label {
            text: {
                switch (changeSecurityDialog.oldMethod) {
                case "passcode":
                    return i18n.tr("Existing passcode")
                case "password":
                    return i18n.tr("Existing passphrase")
                }

                i18n.tr("Existing %1".arg(changeSecurityDialog.oldMethod))
            visible: currentInput.visible
        }

        TextField {
            id: currentInput
            echoMode: TextInput.Password
            inputMethodHints: {
                if (changeSecurityDialog.oldMethod === "password")
                    return Qt.ImhNoAutoUppercase | Qt.ImhSensitiveData
                else if (changeSecurityDialog.oldMethod === "passcode")
                    return Qt.ImhNoAutoUppercase |
                           Qt.ImhSensitiveData |
                           Qt.ImhDigitsOnly
                else
                    return Qt.ImhNone
            }
            inputMask: {
                if (changeSecurityDialog.oldMethod === "passcode")
                    return "9999"
                else
                    return ""
            }
            visible: changeSecurityDialog.oldMethod === "password" ||
                     changeSecurityDialog.oldMethod === "passcode"
        }

        Label {
            text: i18n.tr("Choose %1".arg(changeSecurityDialog.newMethod))
            visible: newInput.visible
        }

        TextField {
            id: newInput
            echoMode: TextInput.Password
            inputMethodHints: {
                if (changeSecurityDialog.newMethod === "password")
                    return Qt.ImhNoAutoUppercase | Qt.ImhSensitiveData
                else if (changeSecurityDialog.newMethod === "passcode")
                    return Qt.ImhNoAutoUppercase |
                           Qt.ImhSensitiveData |
                           Qt.ImhDigitsOnly
                else
                    return Qt.ImhNone
            }
            inputMask: {
                if (changeSecurityDialog.newMethod === "passcode")
                    return "9999"
                else
                    return ""
            }
            visible: changeSecurityDialog.newMethod === "passcode" ||
                     changeSecurityDialog.newMethod === "password"
        }

        Label {
            text: i18n.tr("Confirm %1".arg(changeSecurityDialog.newMethod))
            visible: confirmInput.visible
        }

        TextField {
            id: confirmInput
            echoMode: TextInput.Password
            inputMethodHints: {
                if (changeSecurityDialog.newMethod === "password")
                    return Qt.ImhNoAutoUppercase | Qt.ImhSensitiveData
                else if (changeSecurityDialog.newMethod === "passcode")
                    return Qt.ImhNoAutoUppercase |
                           Qt.ImhSensitiveData |
                           Qt.ImhDigitsOnly
                else
                    return Qt.ImhNone
            }
            inputMask: {
                if (changeSecurityDialog.newMethod === "passcode")
                    return "9999"
                else
                    return ""
            }
            visible: changeSecurityDialog.newMethod === "passcode" ||
                     changeSecurityDialog.newMethod === "password"
        }

        Button {
            text: changeSecurityDialog.newMethod === "swipe" ?
                      i18n.tr("Unset") :
                      i18n.tr("Continue")
            enabled: newInput.text == confirmInput.text
            onClicked: {
                PopupUtils.close(changeSecurityDialog)
                //TODO: Check it's correct before updating and do the update
                settingsSchema.unlockMethod = changeSecurityDialog.newMethod
                changeSecurityDialog.clearInputs()
            }

        }

        Button {
            text: i18n.tr("Cancel")
            onClicked: {
                PopupUtils.close(changeSecurityDialog)
                unlockMethod.skip = true
                unlockMethod.selectedIndex = getUnlockMethod()
                changeSecurityDialog.clearInputs()
            }
        }
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.Standard {
            text: i18n.tr("Unlock the phone using:")
        }

        ListItem.ValueSelector {
            property string swipe: i18n.tr("Swipe (no security)")
            property string passcode: i18n.tr("4-digit passcode")
            property string passphrase: i18n.tr("Passphrase")
            property string swipeAlt: i18n.tr("Swipe (no security)… ")
            property string passcodeAlt: i18n.tr("4-digit passcode…")
            property string passphraseAlt: i18n.tr("Passphrase…")

            property bool skip: true
            property bool firstRun: true

            id: unlockMethod
            values: [
                selectedIndex == 0 ? swipe : swipeAlt,
                selectedIndex == 1 ? passcode : passcodeAlt,
                selectedIndex == 2 ? passphrase : passphraseAlt
            ]
            expanded: true
            onExpandedChanged: expanded = true
            onSelectedIndexChanged: {
                if (getUnlockMethod() === 0 && firstRun) { // swipe
                    changeSecurityDialog.show()
                    firstRun = false
                }

                // Otherwise the dialogs pop up the first time
                if (skip) {
                    skip = false
                    return
                }

                changeSecurityDialog.show()
            }
        }
        Binding {
            target: unlockMethod
            property: "selectedIndex"
            value: getUnlockMethod()
        }

        ListItem.SingleControl {
            control: Button {
                text: i18n.tr("Change passcode…")
                width: parent.width - units.gu(4)
                enabled: false //unlockMethod.selectedIndex == 1 //passcode
            }
        }
    }
}
