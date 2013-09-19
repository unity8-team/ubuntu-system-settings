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
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import SystemSettings 1.0
import Ubuntu.SystemSettings.Diagnostics 1.0
import Ubuntu.SystemSettings.SecurityPrivacy 1.0

ItemPage {
    id: root

    title: i18n.tr("Security & Privacy")
    flickable: scrollWidget

    UbuntuDiagnostics {
        id: diagnosticsWidget
    }

    UbuntuSecurityPrivacyPanel {
        id: securityPrivacy
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

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Standard {
                text: i18n.tr("Security:")
            }
/* Enable once we actually support phone locking (i.e. have a LightDM greeter)
            ListItem.SingleValue {
                text: i18n.tr("Phone locking")
                value: i18n.tr("1 minute",
                               "%1 minutes".arg(5),
                               5)
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("PhoneLocking.qml"))
            }
*/
            ListItem.SingleValue {
                text: i18n.tr("SIM PIN")
                value: "Off"
                progression: true
            }
            ListItem.Standard {
                text: i18n.tr("Privacy:")
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
            ListItem.SingleValue {
                text: i18n.tr("Location access")
                value: "On"
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("Location.qml"))
            }
            ListItem.SingleValue {
                text: i18n.tr("Other app access")
                progression: true
            }
            ListItem.SingleValue {
                text: i18n.tr("Diagnostics")
                progression: true
                value: diagnosticsWidget.canReportCrashes ?
                           i18n.tr("Sent") :
                           i18n.tr("Not sent")
                onClicked: {
                    var path = "../diagnostics/PageComponent.qml";
                    pageStack.push(Qt.resolvedUrl(path));
                }
            }

        }
    }
}
