/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 0.1
import Ubuntu.SystemSettings.SecurityPrivacy 1.0
import "../Components" as LocalComponents

/**
 * One quirk with this page: we don't actually set the password.  We avoid
 * doing it here because the user can come back to this page and change their
 * answer.  We don't run as root, so if we did set the password immediately,
 * we'd need to prompt for their previous password when they came back and
 * changed their answer.  Which is silly UX.  So instead, we just keep track
 * of their choice and set the password at the end (see main.qml).
 * Setting the password shouldn't fail, since Ubuntu Touch has loose password
 * requirements, but we'll check what we can here.  Ideally we'd be able to ask
 * the system if a password is legal without actually setting that password.
 */

LocalComponents.Page {
    id: passwdPage
    title: i18n.tr("Set lock security")
    forwardButtonSourceComponent: forwardButton

    readonly property int method: indexToMethod(listview.currentIndex)
    readonly property string password: {
        if (method !== UbuntuSecurityPrivacyPanel.Swipe)
            return passwordInput.text
        else
            return ""
    }

    function indexToMethod(index) {
        if (index === 0)
            return UbuntuSecurityPrivacyPanel.Swipe
        else if (index === 1)
            return UbuntuSecurityPrivacyPanel.Passcode
        else
            return UbuntuSecurityPrivacyPanel.Passphrase
    }

    Column {
        id: content
        spacing: units.gu(2)
        anchors {
            fill: parent
            topMargin: passwdPage.topMargin
            bottomMargin: passwdPage.bottomMargin
            leftMargin: passwdPage.leftMargin
            rightMargin: passwdPage.rightMargin
        }

        ComboButton {
            id: combo

            text: listview.currentItem.text
            onClicked: expanded = !expanded
            UbuntuListView {
                id: listview
                width: parent.width
                height: combo.comboListHeight
                model: 3
                currentIndex: 1
                delegate: Standard {
                    text: {
                        var method = indexToMethod(modelData)
                        if (method === UbuntuSecurityPrivacyPanel.Swipe)
                            return "Swipe"
                        else if (method === UbuntuSecurityPrivacyPanel.Passcode)
                            return "Passcode"
                        else
                            return "Passphrase"
                    }
                    onClicked: {
                        listview.currentIndex = index
                        combo.expanded = false
                    }
                }
            }
        }

        TextField {
            id: passwordInput
            echoMode: TextInput.Password
            inputMethodHints: {
                var hints = Qt.ImhNoAutoUppercase | Qt.ImhSensitiveData
                if (passwdPage.method === UbuntuSecurityPrivacyPanel.Passcode)
                    hints |= Qt.ImhDigitsOnly
                return hints
            }
            inputMask: {
                if (passwdPage.method === UbuntuSecurityPrivacyPanel.Passcode)
                    return "9999"
                else
                    return ""
            }
            visible: passwdPage.method !== UbuntuSecurityPrivacyPanel.Swipe
        }

        TextField {
            id: confirmInput
            echoMode: passwordInput.echoMode
            inputMethodHints: passwordInput.inputMethodHints
            inputMask: passwordInput.inputMask
            visible: passwordInput.visible
        }

        Label {
            id: problem
            width: parent.width
            wrapMode: Text.Wrap
            visible: text !== ""
            color: UbuntuColors.red
            text: {
                if (passwordInput.visible) {
                    if (passwordInput.text !== confirmInput.text) {
                        if (passwdPage.method === UbuntuSecurityPrivacyPanel.Passcode)
                            return i18n.tr("Those passcodes don't match.")
                        else
                            return i18n.tr("Those passphrases don't match.")
                    } else if (passwordInput.text.length < 4) {
                        // Note that the number four comes from PAM settings,
                        // which we don't have a good way to interrogate.  We
                        // only do this matching instead of PAM because we want
                        // to set the password via PAM in a different place
                        // than this page.  See comments at top of file.
                        if (passwdPage.method === UbuntuSecurityPrivacyPanel.Passcode)
                            return i18n.tr("Passcode must be at least four digits long.")
                        else
                            return i18n.tr("Passphrase must be at least four characters long.")
                    }
                }
                return ""
            }
        }
    }

    Component {
        id: forwardButton
        LocalComponents.ForwardButton {
            text: i18n.tr("Continue")
            enabled: !problem.visible
            onClicked: {
                root.passwordMethod = passwdPage.method
                root.password = passwdPage.password
                pageStack.next()
            }
        }
    }
}
