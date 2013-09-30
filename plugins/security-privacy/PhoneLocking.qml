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
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.SecurityPrivacy 1.0

ItemPage {
    title: i18n.tr("Phone locking")

    UbuntuSecurityPrivacyPanel {
        id: securityPrivacy
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.SingleValue {
            property string swipe: i18n.tr("None")
            property string passcode: i18n.tr("Passcode")
            property string passphrase: i18n.tr("Passphrase")

            text: i18n.tr("Lock security")
            value: {
                switch (securityPrivacy.securityType) {
                    case UbuntuSecurityPrivacyPanel.Swipe:
                        return swipe
                    case UbuntuSecurityPrivacyPanel.Passcode:
                        return passcode
                    case UbuntuSecurityPrivacyPanel.Passphrase:
                        return passphrase
                }
            }
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("LockSecurity.qml"))
        }

        ListItem.SingleValue {
            property bool lockOnSuspend: securityPrivacy.securityType !==
                                            UbuntuSecurityPrivacyPanel.Swipe
            text: lockOnSuspend ? i18n.tr("Lock when idle")
                                : i18n.tr("Sleep when idle")
            // TRANSLATORS: %1 is the number of minutes
            value: i18n.tr("1 minute",
                           "%1 minutes".arg(5),
                           5)
            progression: true
            onClicked:
                pageStack.push(
                    Qt.resolvedUrl("../battery/SleepValues.qml"),
                    { title: text, lockOnSuspend: lockOnSuspend } )
        }

        SettingsCheckEntry {
            checkStatus: true
            textEntry: i18n.tr("Sleep locks immediately")
            enabled: false /* TODO: enable when there is a backend */
        }

    /* TODO: once we know how to do this
    ListItem.Standard {
        text: i18n.tr("When locked, allow:")
    }
    Launcher,
    Camera,
    ...
    */
    }
}
