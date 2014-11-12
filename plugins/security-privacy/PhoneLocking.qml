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
    id: root
    objectName: "phoneLockingPage"
    title: i18n.tr("Phone locking")

    property bool usePowerd
    property variant powerSettings

    UbuntuSecurityPrivacyPanel {
        id: securityPrivacy
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.SingleValue {
            property string swipe: qsTr("None", "Unlock with swipe")
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
            objectName: "lockTimeout"
            property bool lockOnSuspend: securityPrivacy.securityType !==
                                            UbuntuSecurityPrivacyPanel.Swipe
            text: lockOnSuspend ? i18n.tr("Lock when idle")
                                : i18n.tr("Sleep when idle")
            value: {
                if (usePowerd) {
                    var timeout = Math.round(powerSettings.activityTimeout/60)
                    return (powerSettings.activityTimeout != 0) ?
                                // TRANSLATORS: %1 is the number of minutes
                                i18n.tr("%1 minute",
                                        "%1 minutes",
                                        timeout).arg(timeout) :
                                i18n.tr("Never")
                }
                else {
                    var timeout = Math.round(powerSettings.idleDelay/60)
                    return (powerSettings.idleDelay != 0) ?
                                // TRANSLATORS: %1 is the number of minutes
                                i18n.tr("%1 minute",
                                        "%1 minutes",
                                        timeout).arg(timeout) :
                                i18n.tr("Never")
                }
            }
            progression: true
            onClicked:
                pageStack.push(
                    Qt.resolvedUrl("../battery/SleepValues.qml"),
                    { title: text, lockOnSuspend: lockOnSuspend } )
        }

        ListItem.Standard {
            control: CheckBox {
                checked: true
            }
            text: i18n.tr("Sleep locks immediately")
            visible: showAllUI
        }

        ListItem.Standard {
            text: i18n.tr("When locked, allow:")
        }

        ListItem.Standard {
            text: i18n.tr("Launcher")
            control: CheckBox {
                id: launcherCheck
                enabled: securityPrivacy.securityType !== UbuntuSecurityPrivacyPanel.Swipe

                property bool serverChecked: securityPrivacy.enableLauncherWhileLocked || !enabled
                onServerCheckedChanged: checked = serverChecked
                Component.onCompleted: checked = serverChecked
                onTriggered: securityPrivacy.enableLauncherWhileLocked = checked
            }
        }

        ListItem.Standard {
            text: i18n.tr("Notifications and quick settings")
            control: CheckBox {
                id: indicatorsCheck
                enabled: securityPrivacy.securityType !== UbuntuSecurityPrivacyPanel.Swipe

                property bool serverChecked: securityPrivacy.enableIndicatorsWhileLocked || !enabled
                onServerCheckedChanged: checked = serverChecked
                Component.onCompleted: checked = serverChecked
                onTriggered: securityPrivacy.enableIndicatorsWhileLocked = checked
            }
        }

        ListItem.Caption {
            text: securityPrivacy.securityType === UbuntuSecurityPrivacyPanel.Swipe ?
                  i18n.tr("Turn on lock security to restrict access when the phone is locked.") :
                  i18n.tr("Other apps and functions will prompt you to unlock.")
        }
    }
}
