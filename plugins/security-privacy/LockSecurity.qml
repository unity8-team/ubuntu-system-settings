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
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Popups 1.0
import Ubuntu.SystemSettings.SecurityPrivacy 1.0
import SystemSettings 1.0

ItemPage {
    id: page
    title: i18n.tr("Lock security")

    UbuntuSecurityPrivacyPanel {
        id: securityPrivacy
    }

    function methodToIndex(method) {
        switch (method) {
            case UbuntuSecurityPrivacyPanel.Swipe:
                return 0
            case UbuntuSecurityPrivacyPanel.Passcode:
                return 1
            case UbuntuSecurityPrivacyPanel.Passphrase:
                return 2
        }
    }

    function indexToMethod(index) {
        switch (index) {
            case 0:
                return UbuntuSecurityPrivacyPanel.Swipe
            case 1:
                return UbuntuSecurityPrivacyPanel.Passcode
            case 2:
                return UbuntuSecurityPrivacyPanel.Passphrase
        }
    }

    function openDialog() {
        var newMethod = indexToMethod(unlockMethod.selectedIndex)
        if (securityPrivacy.trySetSecurity(newMethod))
            return // Oh, that was easy!  No need to prompt

        var dlg = PopupUtils.open(dialogComponent)
        // Set manually rather than have these be dynamically bound, since
        // the security type can change out from under us, but we don't
        // want dialog to change in that case.
        dlg.oldMethod = securityPrivacy.securityType
        dlg.newMethod = newMethod
    }

    Component {
        id: dialogComponent

        Dialog {
            id: changeSecurityDialog

            // This is a bit hacky, but the contents of this dialog get so tall
            // that on a mako device, they don't fit with the OSK also visible.
            // So we scrunch up spacing.
            Binding {
                target: __foreground
                property: "itemSpacing"
                value: units.gu(1)
            }

            property int oldMethod
            property int newMethod

            title: {
                if (changeSecurityDialog.newMethod ==
                        changeSecurityDialog.oldMethod) { // Changing existing
                    switch (changeSecurityDialog.newMethod) {
                    case UbuntuSecurityPrivacyPanel.Passcode:
                        return i18n.tr("Change PIN code…")
                    case UbuntuSecurityPrivacyPanel.Passphrase:
                        return i18n.tr("Change passphrase…")
                    default: // To stop the runtime complaining
                        return ""
                    }
                } else {
                    switch (changeSecurityDialog.newMethod) {
                    case UbuntuSecurityPrivacyPanel.Swipe:
                        return i18n.tr("Switch to swipe")
                    case UbuntuSecurityPrivacyPanel.Passcode:
                        return i18n.tr("Switch to PIN code")
                    case UbuntuSecurityPrivacyPanel.Passphrase:
                        return i18n.tr("Switch to passphrase")
                    }
                }
            }

            Label {
                text: {
                    switch (changeSecurityDialog.oldMethod) {
                    case UbuntuSecurityPrivacyPanel.Passcode:
                        return i18n.tr("Existing PIN code")
                    case UbuntuSecurityPrivacyPanel.Passphrase:
                        return i18n.tr("Existing passphrase")
                    // Shouldn't be reached when visible but still evaluated
                    default:
                        return ""
                    }
                }

                visible: currentInput.visible
            }

            TextField {
                id: currentInput
                echoMode: TextInput.Password
                inputMethodHints: {
                    if (changeSecurityDialog.oldMethod ===
                            UbuntuSecurityPrivacyPanel.Passphrase)
                        return Qt.ImhNoAutoUppercase | Qt.ImhSensitiveData
                    else if (changeSecurityDialog.oldMethod ===
                             UbuntuSecurityPrivacyPanel.Passcode)
                        return Qt.ImhNoAutoUppercase |
                               Qt.ImhSensitiveData |
                               Qt.ImhDigitsOnly
                    else
                        return Qt.ImhNone
                }
                inputMask: {
                    if (changeSecurityDialog.oldMethod ===
                            UbuntuSecurityPrivacyPanel.Passcode)
                        return "9999"
                    else
                        return ""
                }
                visible: changeSecurityDialog.oldMethod ===
                            UbuntuSecurityPrivacyPanel.Passphrase ||
                         changeSecurityDialog.oldMethod ===
                             UbuntuSecurityPrivacyPanel.Passcode
                onTextChanged: {
                    if (changeSecurityDialog.newMethod ===
                            UbuntuSecurityPrivacyPanel.Swipe)
                        confirmButton.enabled = text.length > 0
                }
            }

            Label {
                id: incorrect
                text: ""
                visible: text !== ""
                color: "darkred"
            }

            Label {
                text: {
                    switch (changeSecurityDialog.newMethod) {
                    case UbuntuSecurityPrivacyPanel.Passcode:
                        return i18n.tr("Choose PIN code")
                    case UbuntuSecurityPrivacyPanel.Passphrase:
                        return i18n.tr("Choose passphrase")
                    // Shouldn't be reached when visible but still evaluated
                    default:
                        return ""
                    }
                }
                visible: newInput.visible
            }

            TextField {
                id: newInput
                echoMode: TextInput.Password
                inputMethodHints: {
                    if (changeSecurityDialog.newMethod ===
                            UbuntuSecurityPrivacyPanel.Passphrase)
                        return Qt.ImhNoAutoUppercase | Qt.ImhSensitiveData
                    else if (changeSecurityDialog.newMethod ===
                             UbuntuSecurityPrivacyPanel.Passcode)
                        return Qt.ImhNoAutoUppercase |
                               Qt.ImhSensitiveData |
                               Qt.ImhDigitsOnly
                    else
                        return Qt.ImhNone
                }
                inputMask: {
                    if (changeSecurityDialog.newMethod ===
                            UbuntuSecurityPrivacyPanel.Passcode)
                        return "9999"
                    else
                        return ""
                }
                visible: changeSecurityDialog.newMethod ===
                            UbuntuSecurityPrivacyPanel.Passcode ||
                         changeSecurityDialog.newMethod ===
                            UbuntuSecurityPrivacyPanel.Passphrase
                // Doesn't get updated if you set this in enabled of confirmButton
                onTextChanged: confirmButton.enabled =
                               (acceptableInput && (!visible || text.length > 0))
            }

            Label {
                text: {
                    switch (changeSecurityDialog.newMethod) {
                    case UbuntuSecurityPrivacyPanel.Passcode:
                        return i18n.tr("Confirm PIN code")
                    case UbuntuSecurityPrivacyPanel.Passphrase:
                        return i18n.tr("Confirm passphrase")
                    // Shouldn't be reached when visible but still evaluated
                    default:
                        return ""
                    }
                }
                visible: confirmInput.visible
            }

            TextField {
                id: confirmInput
                echoMode: TextInput.Password
                inputMethodHints: {
                    if (changeSecurityDialog.newMethod ===
                            UbuntuSecurityPrivacyPanel.Passphrase)
                        return Qt.ImhNoAutoUppercase | Qt.ImhSensitiveData
                    else if (changeSecurityDialog.newMethod ===
                             UbuntuSecurityPrivacyPanel.Passcode)
                        return Qt.ImhNoAutoUppercase |
                               Qt.ImhSensitiveData |
                               Qt.ImhDigitsOnly
                    else
                        return Qt.ImhNone
                }
                inputMask: {
                    if (changeSecurityDialog.newMethod ===
                            UbuntuSecurityPrivacyPanel.Passcode)
                        return "9999"
                    else
                        return ""
                }
                visible: changeSecurityDialog.newMethod ===
                            UbuntuSecurityPrivacyPanel.Passcode ||
                         changeSecurityDialog.newMethod ===
                            UbuntuSecurityPrivacyPanel.Passphrase
            }

            Label {
                id: notMatching
                wrapMode: Text.Wrap
                text: {
                    if (changeSecurityDialog.newMethod ===
                            UbuntuSecurityPrivacyPanel.Passcode)
                        return i18n.tr("Those PIN codes don't match. Try again.")
                    if (changeSecurityDialog.newMethod ===
                            UbuntuSecurityPrivacyPanel.Passphrase)
                        return i18n.tr("Those passphrases don't match. Try again.")

                    //Fallback to prevent warnings. Not displayed.
                    return ""
                }
                visible: false
                color: "darkred"
            }

            RowLayout {
                spacing: units.gu(1)

                Button {
                    Layout.fillWidth: true
                    color: UbuntuColors.lightGrey
                    text: i18n.tr("Cancel")
                    onClicked: {
                        PopupUtils.close(changeSecurityDialog)
                        unlockMethod.selectedIndex =
                                methodToIndex(securityPrivacy.securityType)
                    }
                }

                Button {
                    id: confirmButton
                    Layout.fillWidth: true
                    color: UbuntuColors.green

                    text: {
                        if (changeSecurityDialog.newMethod ===
                                UbuntuSecurityPrivacyPanel.Swipe)
                            return i18n.tr("Unset")
                        else if (changeSecurityDialog.oldMethod ===
                                changeSecurityDialog.newMethod)
                            return i18n.tr("Change")
                        else
                            return i18n.tr("Set")
                    }
                    enabled: false
                    onClicked: {
                        changeSecurityDialog.enabled = false

                        var match = (newInput.text == confirmInput.text)
                        notMatching.visible = !match
                        if (!match) {
                            changeSecurityDialog.enabled = true
                            newInput.forceActiveFocus()
                            newInput.selectAll()
                            return
                        }

                        var errorText = securityPrivacy.setSecurity(
                            currentInput.visible ? currentInput.text : "",
                            newInput.text,
                            changeSecurityDialog.newMethod)
                        incorrect.text = errorText
                        if (errorText !== "") {
                            changeSecurityDialog.enabled = true
                            currentInput.forceActiveFocus()
                            currentInput.selectAll()
                            return
                        }

                        changeSecurityDialog.enabled = true
                        PopupUtils.close(changeSecurityDialog)
                    }
                }
            }
        }
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.Standard {
            text: i18n.tr("Unlock the phone using:")
        }

        ListItem.ItemSelector {
            property string swipe: i18n.tr("Swipe (no security)")
            property string passcode: i18n.tr("4-digit PIN code")
            property string passphrase: i18n.tr("Passphrase")
            property string swipeAlt: i18n.tr("Swipe (no security)… ")
            property string passcodeAlt: i18n.tr("4-digit PIN code…")
            property string passphraseAlt: i18n.tr("Passphrase…")

            id: unlockMethod
            model: 3
            delegate: OptionSelectorDelegate {
                text: index == 0 ? (unlockMethod.selectedIndex == 0 ? unlockMethod.swipe : unlockMethod.swipeAlt) :
                     (index == 1 ? (unlockMethod.selectedIndex == 1 ? unlockMethod.passcode : unlockMethod.passcodeAlt) :
                                   (unlockMethod.selectedIndex == 2 ? unlockMethod.passphrase : unlockMethod.passphraseAlt))
            }
            expanded: true
            onDelegateClicked: {
                if (selectedIndex === index && !changeControl.visible)
                    return // nothing to do

                selectedIndex = index
                openDialog()
            }
        }
        Binding {
            target: unlockMethod
            property: "selectedIndex"
            value: methodToIndex(securityPrivacy.securityType)
        }

        ListItem.SingleControl {

            id: changeControl
            visible: securityPrivacy.securityType !==
                        UbuntuSecurityPrivacyPanel.Swipe

            control: Button {
                property string changePasscode: i18n.tr("Change PIN code…")
                property string changePassphrase: i18n.tr("Change passphrase…")

                property bool passcode: securityPrivacy.securityType ===
                                        UbuntuSecurityPrivacyPanel.Passcode

                enabled: parent.visible

                text: passcode ? changePasscode : changePassphrase
                width: parent.width - units.gu(4)

                onClicked: openDialog()
            }
            showDivider: false
        }
    }
}
