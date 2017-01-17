/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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

import Biometryd 0.0
import GSettings 1.0
import QMenuModel 0.1
import QtQuick 2.4
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import SystemSettings 1.0
import Ubuntu.Settings.Fingerprint 0.1
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

    onPushedOntoStack: {
        var page;
        var opts = {
            pluginManager: pluginManager, pluginOptions: pluginOptions,};
        if (pluginOptions && pluginOptions['subpage']) {
            switch (pluginOptions['subpage']) {
            case 'location':
                page = Qt.resolvedUrl("Location.qml");
                break;
            case 'permissions':
                page = Qt.resolvedUrl("AppAccess.qml")
                break;
            }
        } else if (pluginOptions && pluginOptions['service']) {
            // This whole else if branch will be removed once the
            // camera app asks for [1] as described in lp:1545733.
            // [1] settings:///system/permissions?service=camera
            page = Qt.resolvedUrl("AppAccess.qml");
        }
        if (page) {
            pageStack.addPageToNextColumn(root, page, opts);
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

            SettingsItemTitle {
                id: securityTitle
                text: i18n.tr("Security")
            }

            SettingsListItems.SingleValueProgression {
                id: fingerprintControl
                objectName: "fingerprintControl"
                text: i18n.tr("Fingerprint ID")
                onClicked: pageStack.addPageToNextColumn(root, fingeprintPage, {
                    passcodeSet: securityPrivacy.securityType !== UbuntuSecurityPrivacyPanel.Swipe
                })
                visible: Biometryd.available || showAllUI
            }

            Component {
                id: fingeprintPage
                Fingerprints {
                    onRequestPasscode: {
                        pageStack.removePages(root);
                        pageStack.addPageToNextColumn(root, Qt.resolvedUrl("LockSecurity.qml"));
                    }
                }
            }

            SettingsListItems.SingleValueProgression {
                id: lockingControl
                objectName: "lockingControl"
                text: i18n.tr("Locking and unlocking")
                onClicked: pageStack.addPageToNextColumn(root, Qt.resolvedUrl("PhoneLocking.qml"), {
                    usePowerd: usePowerd,
                    powerSettings: powerSettings
                })
            }

            SettingsListItems.SingleValueProgression {
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
                onClicked: pageStack.addPageToNextColumn(root, Qt.resolvedUrl("SimPin.qml"), { sims: sims })
            }

            SettingsListItems.Standard {
                text: i18n.tr("Encryption")
                Switch {
                    id: encryptionSwitch
                    checked: false
                }
                visible: showAllUI
            }

            ListItems.Caption {
                text: i18n.tr(
                        "Encryption protects against access to phone data when the phone is connected to a PC or other device.")
                visible: showAllUI
            }

            SettingsItemTitle {
                text: i18n.tr("Privacy")
            }

            SettingsListItems.Standard {
                text: i18n.tr("Stats on welcome screen")
                Switch {
                    property bool serverChecked: securityPrivacy.statsWelcomeScreen
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: securityPrivacy.statsWelcomeScreen = checked
                }
            }

            SettingsListItems.Standard {
                text: i18n.tr("Messages on welcome screen")
                Switch {
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

            SettingsListItems.SingleValueProgression {
                id: locationItem
                objectName: "locationItem"
                text: i18n.tr("Location")
                value: ""
                onClicked: pageStack.addPageToNextColumn(root, Qt.resolvedUrl("Location.qml"))
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

            SettingsListItems.SingleValueProgression {
                text: i18n.tr("App permissions")
                onClicked: pageStack.addPageToNextColumn(root, Qt.resolvedUrl("AppAccess.qml"), {pluginManager: pluginManager})
            }

            SettingsListItems.SingleValueProgression {
                text: i18n.tr("Diagnostics")
                value: diagnosticsWidget.reportCrashes ?
                           /* TRANSLATORS: This string is shown when crash
                              reports are to be sent by the system. */
                           i18n.tr("Sent") :
                           /* TRANSLATORS: This string is shown when crash
                              reports are not to be sent by the system */
                           i18n.tr("Not sent")
                onClicked: {
                    var path = "../diagnostics/PageComponent.qml";
                    pageStack.addPageToNextColumn(root, Qt.resolvedUrl(path));
                }
            }
        }
    }
}
