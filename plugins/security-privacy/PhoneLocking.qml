/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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
import QtQuick 2.4
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.SystemSettings.SecurityPrivacy 1.0

ItemPage {
    id: root
    objectName: "phoneLockingPage"
    title: i18n.tr("Locking and unlocking")
    flickable: scrollWidget

    property bool usePowerd
    property var powerSettings

    UbuntuSecurityPrivacyPanel {
        id: securityPrivacy
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
        /* Set the direction to workaround
           https://bugreports.qt-project.org/browse/QTBUG-31905 otherwise the UI
           might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            SettingsListItems.SingleValueProgression {
                visible: !isSnap || showAllUI
                property string swipe: i18n.ctr("Unlock with swipe", "None")
                property string passcode: i18n.tr("Passcode")
                property string passphrase: i18n.tr("Passphrase")
                property string fingerprint: i18n.tr("Fingerprint")

                objectName: "lockSecurity"
                text: i18n.tr("Lock security")
                value: {
                    if (securityPrivacy.enableFingerprintIdentification)
                        return fingerprint
                    switch (securityPrivacy.securityType) {
                        case UbuntuSecurityPrivacyPanel.Swipe:
                            return swipe
                        case UbuntuSecurityPrivacyPanel.Passcode:
                            return passcode
                        case UbuntuSecurityPrivacyPanel.Passphrase:
                            return passphrase
                        case UbuntuSecurityPrivacyPanel.Fingerprint:
                            return fingerprint
                    }
                }
                onClicked: pageStack.addPageToNextColumn(
                    root, Qt.resolvedUrl("LockSecurity.qml"))
            }

            SettingsListItems.SingleValueProgression {
                objectName: "lockTimeout"
                property bool lockOnSuspend: securityPrivacy.securityType !==
                                                UbuntuSecurityPrivacyPanel.Swipe
                text: lockOnSuspend ? i18n.tr("Lock when idle")
                                    : i18n.tr("Sleep when idle")
                value: {
                    if (usePowerd) {
                        var timeout = powerSettings.activityTimeout
                        return timeout == 0 ?
                                    i18n.tr("Never") :
				    (timeout < 60) ?
		                    // TRANSLATORS: %1 is the number of seconds
                    	            i18n.tr("After %1 second",
                               	            "After %1 seconds",
                                             timeout).arg(timeout) :
                                    // TRANSLATORS: %1 is the number of minutes
                                    i18n.tr("After %1 minute",
                                            "After %1 minutes",
                                            Math.round(timeout/60)).arg(Math.round(timeout/60))
                    }
                    else {
                        var timeout = Math.round(powerSettings.idleDelay/60)
                        return (powerSettings.idleDelay != 0) ?
                                    // TRANSLATORS: %1 is the number of minutes
                                    i18n.tr("After %1 minute",
                                            "After %1 minutes",
                                            timeout).arg(timeout) :
                                    i18n.tr("Never")
                    }
                }
                onClicked:
                    pageStack.addPageToNextColumn(root,
                        Qt.resolvedUrl("../battery/SleepValues.qml"),
                        { title: text, lockOnSuspend: lockOnSuspend } )
            }

            SettingsListItems.Standard {
                CheckBox {
                    checked: true
                }
                text: i18n.tr("Sleep locks immediately")
                visible: showAllUI
            }

            SettingsItemTitle {
                id: lockedPermissions
                text: i18n.tr("When locked, allow:")
                visible: !isSnap || showAllUI
            }

            SettingsListItems.Standard {
                visible: lockedPermissions.visible
                text: i18n.tr("Launcher")
                CheckBox {
                    id: launcherCheck
                    enabled: securityPrivacy.securityType !== UbuntuSecurityPrivacyPanel.Swipe

                    property bool serverChecked: securityPrivacy.enableLauncherWhileLocked || !enabled
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: securityPrivacy.enableLauncherWhileLocked = checked
                }
            }

            SettingsListItems.Standard {
                visible: lockedPermissions.visible
                text: i18n.tr("Notifications and quick settings")
                 CheckBox {
                    id: indicatorsCheck
                    enabled: securityPrivacy.securityType !== UbuntuSecurityPrivacyPanel.Swipe

                    property bool serverChecked: securityPrivacy.enableIndicatorsWhileLocked || !enabled
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: securityPrivacy.enableIndicatorsWhileLocked = checked
                }
            }

            ListItems.Caption {
                visible: lockedPermissions.visible
                text: securityPrivacy.securityType === UbuntuSecurityPrivacyPanel.Swipe ?
                      i18n.tr("Turn on lock security to restrict access when the device is locked.") :
                      i18n.tr("Other apps and functions will prompt you to unlock.")
            }
        }
    }
}
