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
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import SystemSettings 1.0
import Ubuntu.SystemSettings.Battery 1.0
import Ubuntu.SystemSettings.Diagnostics 1.0
import Ubuntu.SystemSettings.SecurityPrivacy 1.0
import MeeGo.QOfono 0.2
import "sims.js" as Sims

ItemPage {
    id: root
    objectName: "securityPrivacyPage"

    title: i18n.tr("Security & Privacy")
    flickable: scrollWidget

    property alias usePowerd: batteryBackend.powerdRunning
    property bool lockOnSuspend
    property var modemsSorted: []
    property var sims: []
    /* glue to something that will emit change events
    TODO: fix this so that the present count emits events of its own */
    property int simsPresent: simsLoaded ? Sims.getPresentCount() : 0
    property int simsLoaded: 0
    property int simsLocked: {
        var t = 0;
        sims.forEach(function (sim) {
            if (sim.simMng.lockedPins.length > 0)
                t++;
        });
        return t;
    }

    property var pluginOptions
    Connections {
        target: pageStack
        onCurrentPageChanged: {
            // If we get called with service=something open the app access page
            // for that specific service, if it does exist.
            //
            // We need to wait until the PageComponent has been pushed to the stack
            // before pushing the subpages, otherwise they will be pushed below the
            // PageComponent.
            if (pageStack.currentPage === root) {
                if (pluginOptions && pluginOptions['service']) {
                    var page = pageStack.push(Qt.resolvedUrl("AppAccess.qml"), {pluginManager: pluginManager})
                    page.openService(pluginOptions['service'])
                }
                // Once done, disable this Connections, so that if the user navigates
                // back to the root we won't push the subpages again
                target = null
            }
        }
    }

    UbuntuDiagnostics {
        id: diagnosticsWidget
    }

    UbuntuSecurityPrivacyPanel {
        id: securityPrivacy
    }

    UbuntuBatteryPanel {
        id: batteryBackend
    }

    OfonoManager {
        id: manager
        onModemsChanged: {
            root.modemsSorted = modems.slice(0).sort();
            Sims.createQML();
            root.sims = Sims.getAll();
        }
    }

    GSettings {
        id: phoneSettings
        schema.id: "com.ubuntu.phone"
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

            ListItem.Header {
                id: securityTitle
                text: i18n.tr("Security")
            }
            ListItem.SingleValue {
                id: lockingControl
                objectName: "lockingControl"
                text: i18n.tr("Locking and unlocking")
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("PhoneLocking.qml"), {usePowerd: usePowerd, powerSettings: powerSettings})
            }
            ListItem.SingleValue {
                id: simControl
                objectName: "simControl"
                text: i18n.tr("SIM PIN")
                value: {
                    if (simsLoaded === 1 && simsLocked > 0)
                        return i18n.tr("On");
                    else if (simsLoaded > 1 && simsLocked > 0)
                        return simsLocked + "/" + simsLoaded;
                    else
                        return i18n.tr("Off");
                }
                visible: simsPresent > 0
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("SimPin.qml"), { sims: sims })
            }
            ListItem.Standard {
                text: i18n.tr("Encryption")
                control: Switch {
                    id: encryptionSwitch
                    checked: false
                }
                visible: showAllUI
            }
            ListItem.Caption {
                text: i18n.tr(
                        "Encryption protects against access to phone data when the phone is connected to a PC or other device.")
                visible: showAllUI
            }
            ListItem.Header {
                text: i18n.tr("Privacy")
            }
            ListItem.Standard {
                text: i18n.tr("Stats on welcome screen")
                control: Switch {
                    property bool serverChecked: securityPrivacy.statsWelcomeScreen
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: securityPrivacy.statsWelcomeScreen = checked
                }
            }

            ListItem.Standard {
                text: i18n.tr("Messages on welcome screen")
                control: Switch {
                    property bool serverChecked: securityPrivacy.messagesWelcomeScreen
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: securityPrivacy.messagesWelcomeScreen = checked
                }
                visible: showAllUI
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
                id: locationItem
                objectName: "locationItem"
                text: i18n.tr("Location")
                value: ""
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("Location.qml"))
                visible: true
                enabled: true
                property variant locationEnabled
                onLocationEnabledChanged: {
                    value = locationEnabled ?
                       i18n.tr("On") : i18n.tr("Off")
                }
            }
            Binding {
                target: locationItem
                property: "locationEnabled"
                value: locationActionGroup.enabled.state
            }
            ListItem.SingleValue {
                text: i18n.tr("App permissions")
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("AppAccess.qml"), {pluginManager: pluginManager})
            }
            ListItem.SingleValue {
                text: i18n.tr("Diagnostics")
                progression: true
                value: diagnosticsWidget.reportCrashes ?
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
