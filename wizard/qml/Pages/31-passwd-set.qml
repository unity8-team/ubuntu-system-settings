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
import Ubuntu.SystemSettings.SecurityPrivacy 1.0
import "../Components" as LocalComponents
import "file:///usr/share/unity8/Components" as UnityComponents

/**
 * See the main passwd-type page for an explanation of why we don't actually
 * directly set the password here.
 */

LocalComponents.Page {
    id: passwdSetPage
    forwardButtonSourceComponent: forwardButton

    skip: root.passwordMethod === UbuntuSecurityPrivacyPanel.Swipe

    // If we are entering this page, clear any saved password and get focus
    onEnabledChanged: if (enabled) lockscreen.clear(false)

    UnityComponents.Lockscreen {
        id: lockscreen
        anchors {
            fill: parent
            topMargin: topMargin
            leftMargin: leftMargin
            rightMargin: rightMargin
            bottomMargin: bottomMargin
        }

        infoText: root.passwordMethod === UbuntuSecurityPrivacyPanel.Passphrase ?
                  i18n.tr("Enter passphrase") :
                  i18n.tr("Choose your passcode")

        errorText: i18n.tr("Passphrase must be 4 characters long")

        showEmergencyCallButton: false
        showCancelButton: false
        alphaNumeric: root.passwordMethod === UbuntuSecurityPrivacyPanel.Passphrase
        minPinLength: 4
        maxPinLength: 4

        onEntered: {
            if (passphrase.length >= 4) {
                root.password = passphrase
                pageStack.load(Qt.resolvedUrl("passwd-confirm.qml"))
            } else {
                lockscreen.clear(true)
            }
        }
    }

    Component {
        id: forwardButton
        LocalComponents.StackButton {
            text: i18n.tr("Skip")
            onClicked: {
                root.passwordMethod = UbuntuSecurityPrivacyPanel.Swipe
                pageStack.prev()
                pageStack.next()
            }
            visible: root.passwordMethod === UbuntuSecurityPrivacyPanel.Passcode
        }
    }
}
