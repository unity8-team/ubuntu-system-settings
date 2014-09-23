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
    title: i18n.tr("Unlock security")
    forwardButtonSourceComponent: forwardButton

    function indexToMethod(index) {
        if (index === 0)
            return UbuntuSecurityPrivacyPanel.Swipe
        else if (index === 1)
            return UbuntuSecurityPrivacyPanel.Passcode
        else
            return UbuntuSecurityPrivacyPanel.Passphrase
    }

    function methodToIndex(method) {
        if (method === UbuntuSecurityPrivacyPanel.Swipe)
            return 0
        else if (method === UbuntuSecurityPrivacyPanel.Passcode)
            return 1
        else
            return 2
    }

    Connections {
        target: root
        onPasswordMethodChanged: listview.currentIndex = methodToIndex(root.passwordMethod)
    }

    Column {
        id: column
        spacing: units.gu(4)
        anchors.fill: content

        Label {
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.Wrap
            text: i18n.tr("Please select how you'd like to unlock your phone.  You can choose between a simple swipe, passcode, and passphrase.")
        }

        ComboButton {
            id: combo
            anchors.left: parent.left
            anchors.right: parent.right

            text: listview.currentItem.text

            onClicked: {
                expanded = !expanded
            }

            UbuntuListView {
                id: listview
                width: parent.width
                height: combo.comboListHeight
                model: 3
                currentIndex: methodToIndex(root.passwordMethod)
                delegate: Standard {
                    text: {
                        var method = indexToMethod(modelData)
                        if (method === UbuntuSecurityPrivacyPanel.Swipe)
                            return "<b>" + i18n.tr("Swipe") + "</b> — " +
                                   i18n.tr("Unlock by simply swiping to the left")
                        else if (method === UbuntuSecurityPrivacyPanel.Passcode)
                            return "<b>" + i18n.tr("Passcode") + "</b> — " +
                                   i18n.tr("Numbers only (4 digits)")
                        else
                            return "<b>" + i18n.tr("Passphrase") + "</b> — " +
                                   i18n.tr("Letters, numbers, and phrases")
                    }
                    onClicked: {
                        listview.currentIndex = index
                        combo.expanded = false
                    }
                }
            }
        }
    }

    Component {
        id: forwardButton
        LocalComponents.StackButton {
            text: i18n.tr("Continue")
            onClicked: {
                root.passwordMethod = indexToMethod(listview.currentIndex)
                pageStack.next()
            }
        }
    }
}
