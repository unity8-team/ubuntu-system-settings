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

    function promptOldSecurity() {
        switch (settingsSchema.unlockMethod) {
            case "swipe": // no previous security so prompt for the new one
                promptNewSecurity()
                break
            case "passcode":
                currentPasscodeDialog.show()
                currentPasscodeInput.forceActiveFocus()
                break
            case "password":
                currentPassphraseDialog.show()
                currentPassphraseInput.forceActiveFocus()
                break
        }
    }

    function promptNewSecurity() {
        var idx = unlockMethod.selectedIndex
        if (idx == 0) {
            settingsSchema.unlockMethod = "swipe"
        } else if (idx == 1) { // passcode
            newPasscodeDialog.show()
            newPasscodeInput.forceActiveFocus()
        } else if (idx == 2) { // passphrase
            newPassphraseDialog.show()
            newPassphraseInput.forceActiveFocus()
        }
    }

    Dialog {
        id: currentPassphraseDialog
        title: i18n.tr("Enter passphrase")
        TextField {
            id: currentPassphraseInput
            echoMode: TextInput.Password
            inputMethodHints: Qt.ImhNoAutoUppercase |
                              Qt.ImhSensitiveData
        }
        Button {
            text: i18n.tr("Continue")
            onClicked: {
                PopupUtils.close(currentPassphraseDialog)
                // TODO: check password
                promptNewSecurity()
            }
        }
        Button {
            text: i18n.tr("Cancel")
            onClicked: {
                PopupUtils.close(currentPassphraseDialog)
                unlockMethod.skip = true
                unlockMethod.selectedIndex = getUnlockMethod()
            }
        }
    }

    Dialog {
        id: currentPasscodeDialog
        title: i18n.tr("Enter passcode")
        TextField {
            id: currentPasscodeInput
            echoMode: TextInput.Password
            inputMethodHints: Qt.ImhNoAutoUppercase |
                              Qt.ImhSensitiveData |
                              Qt.ImhDigitsOnly
            inputMask: "9999"
        }
        Button {
            text: i18n.tr("Continue")
            onClicked: {
                PopupUtils.close(currentPasscodeDialog)
                promptNewSecurity()
            }
        }
        Button {
            text: i18n.tr("Cancel")
            onClicked: {
                PopupUtils.close(currentPasscodeDialog)
                unlockMethod.skip = true
                unlockMethod.selectedIndex = getUnlockMethod()
            }
        }
    }

    Dialog {
        id: newPassphraseDialog
        title: i18n.tr("Choose passphrase")
        text: i18n.tr("New passphrase")
        TextField {
            id: newPassphraseInput
            echoMode: TextInput.Password
            inputMethodHints: Qt.ImhNoAutoUppercase |
                              Qt.ImhSensitiveData
        }
        Button {
            text: i18n.tr("Set")
            onClicked: {
                settingsSchema.unlockMethod = "password"
                PopupUtils.close(newPassphraseDialog)
            }
        }
        Button {
            text: i18n.tr("Cancel")
            onClicked: {
                PopupUtils.close(newPassphraseDialog)
                unlockMethod.skip = true
                unlockMethod.selectedIndex = getUnlockMethod()
            }
        }
    }

    Dialog {
        id: newPasscodeDialog
        title: i18n.tr("Choose passcode")
        text: i18n.tr("New passcode")
        TextField {
            id: newPasscodeInput
            echoMode: TextInput.Password
            inputMethodHints: Qt.ImhNoAutoUppercase |
                              Qt.ImhSensitiveData |
                              Qt.ImhDigitsOnly
            inputMask: "9999"
        }
        Button {
            text: i18n.tr("Set")
            onClicked: {
                settingsSchema.unlockMethod = "passcode"
                PopupUtils.close(newPasscodeDialog)
            }
        }
        Button {
            text: i18n.tr("Cancel")
            onClicked: {
                PopupUtils.close(newPasscodeDialog)
                unlockMethod.skip = true
                unlockMethod.selectedIndex = getUnlockMethod()
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

            signal changed

            id: unlockMethod
            values: [
                selectedIndex == 0 ? swipe : swipeAlt,
                selectedIndex == 1 ? passcode : passcodeAlt,
                selectedIndex == 2 ? passphrase : passphraseAlt
            ]
            expanded: true
            onExpandedChanged: expanded = true
            onSelectedIndexChanged: {
                if (getUnlockMethod() === 0) // swipe
                    promptOldSecurity()

                // Otherwise the dialogs pop up the first time
                if (skip) {
                    skip = false
                    return
                }

                // Prompt for the current passcode or passphrase as necessary
                // This will also prompt for the new one
                promptOldSecurity();
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
