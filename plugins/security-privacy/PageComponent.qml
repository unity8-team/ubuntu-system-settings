/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Evan Dandrea <evan.dandrea@canonical.com>
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
import QMenuModel 0.1
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import SystemSettings 1.0
import Ubuntu.SystemSettings.Battery 1.0
import Ubuntu.SystemSettings.Diagnostics 1.0
import Ubuntu.SystemSettings.SecurityPrivacy 1.0

ItemPage {
    id: root

    title: i18n.tr("Security & Privacy")
    flickable: scrollWidget

    property alias usePowerd: batteryBackend.powerdRunning
    property bool lockOnSuspend
    property variant idleValues: [60,120,180,240,300,600]
    property var timeOut

    UbuntuDiagnostics {
        id: diagnosticsWidget
    }

    UbuntuSecurityPrivacyPanel {
        id: securityPrivacy
    }

    UbuntuBatteryPanel {
        id: batteryBackend
    }

    GSettings {
        id: unitySettings
        schema.id: "com.canonical.Unity.Lenses"
        onChanged: {
            if (key == "remoteContentSearch")
                if (value == 'all')
                    dashSearchId.value = i18n.tr("Phone and Internet")
                else
                    dashSearchId.value = i18n.tr("Phone only")
        }
    }

    GSettings {
        id: powerSettings
        schema.id: usePowerd ? "com.ubuntu.touch.system" : "org.gnome.desktop.session"
    }

    Binding {
        target: root
        property: "timeOut"
        value: usePowerd ? powerSettings.activityTimeout : powerSettings.idleDelay
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: Flickable.StopAtBounds
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Standard {
                id: securityTitle
                text: i18n.tr("Security")
            }            
            ListItem.SingleValue {
                id: lockingControl
                text: i18n.tr("Lock phone")
                // TRANSLATORS: %1 is the number of minutes
                value: timeOut === 0 ? i18n.tr("Manually") :
                       i18n.tr("After %1 minute",
                               "After %1 minutes",
                               idleValues.indexOf(timeOut)).arg(timeOut/60)
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("PhoneLocking.qml"), {idleValues: idleValues, usePowerd: usePowerd, powerSettings: powerSettings})
            }
            ListItem.SingleValue {
                id: simControl
                text: i18n.tr("SIM PIN")
                value: "Off"
                progression: true
                visible: showAllUI
                /* Not implemented yet */
                //onClicked: pageStack.push(Qt.resolvedUrl("SimPin.qml"))
            }
            ListItem.Standard {
                text: i18n.tr("Encryption")
                control: Switch {
                    id: encryptionSwitch
                    checked: false
                }
            }
            ListItem.Caption {
                text: i18n.tr(
                        "Encryption protects against access to phone data when the phone is connected to a PC or other device.")
            }
            ListItem.Standard {
                text: i18n.tr("Privacy")
            }
            ListItem.Standard {
                text: i18n.tr("Stats on welcome screen")
                control: Switch {
                    id: welcomeStatsSwitch
                    checked: securityPrivacy.statsWelcomeScreen
                }
            }
            Binding {
                target: securityPrivacy
                property: "statsWelcomeScreen"
                value: welcomeStatsSwitch.checked
            }

            ListItem.Standard {
                text: i18n.tr("Messages on welcome screen")
                control: Switch {
                    id: welcomeMessagesSwitch
                    checked: securityPrivacy.messagesWelcomeScreen
                }
                visible: showAllUI
            }
            Binding {
                target: securityPrivacy
                property: "messagesWelcomeScreen"
                value: welcomeMessagesSwitch.checked
            }

            ListItem.SingleValue {
                id: dashSearchId
                text: i18n.tr("Dash search")
                value: (unitySettings.remoteContentSearch === 'all') ?
                           i18n.tr("Phone and Internet") :
                           i18n.tr("Phone only")
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("Dash.qml"))
            }
            QDBusActionGroup {
                id: locationActionGroup
                busType: DBus.SessionBus
                busName: "com.canonical.indicator.location"
                objectPath: "/com/canonical/indicator/location"

                property variant enabled: action("location-detection-enabled")

                Component.onCompleted: start()
            }
            ListItem.SingleValue {
                text: i18n.tr("Location access")
                value: "On"
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("Location.qml"))
                visible: showAllUI && // Hidden until the indicator works
                         locationActionGroup.enabled.state !== undefined
            }
            ListItem.SingleValue {
                text: i18n.tr("Other app access")
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("AppAccess.qml"))
            }
            ListItem.SingleValue {
                text: i18n.tr("Diagnostics")
                progression: true
                value: diagnosticsWidget.canReportCrashes ?
                           /* TRANSLATORS: This string is shown when crash
                              reports are to be sent by the system. */
                           i18n.tr("Sent") :
                           /* TRANSLATORS: This string is shown when crash
                              reports are not to be sent by the system */
                           i18n.tr("Not sent")
                onClicked: {
                    var path = "../diagnostics/PageComponent.qml";
                    pageStack.push(Qt.resolvedUrl(path));
                }
            }
        }
    }
}
