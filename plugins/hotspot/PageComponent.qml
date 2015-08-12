/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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

import QtQuick 2.0
import QMenuModel 0.1
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.Components.Popups 0.1
import Ubuntu.Connectivity 1.0
import Ubuntu.Settings.Components 0.1 as USC

/* This is a temporary solution to the issue of Hotspots failing on mako. If
the device is mako, we hide the hotspot entry. Will be removed once lp:1434591
has been resolved. */
import Ubuntu.SystemSettings.Update 1.0

ItemPage {

    id: root
    objectName: "hotspotPage"
    title: i18n.tr("Hotspot")

    // We disable the back action while a hotspot is in the process of either
    // being enabled or disabled.
    head.backAction: Action {
        iconName: "back"
        enabled: !Connectivity.unstoppableOperationHappening
        onTriggered: pageStack.pop()
    }

    states: [
        State {
            name: "disabled"
            // Undefined WifiEnabled means Connectivity is unavailable.
            when: (typeof Connectivity.WifiEnabled === "undefined" &&
                   UpdateManager.deviceName !== "mako")
            PropertyChanges {
                target: hotspotItem
                enabled: false
            }
            PropertyChanges {
                target: hotspotSetupButton
                enabled: false
            }
        },
        State {
            name: "nowifi"
            when: (typeof Connectivity.WifiEnabled === "boolean" &&
                   !Connectivity.WifiEnabled)
            PropertyChanges {
                target: hotspotSwitchWhenWifiDisabled
                visible: true
            }
        }
    ]

    Loader {
        id: setup
        asynchronous: false
    }

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
            Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {

            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(2)

            ListItem.Standard {
                id: hotspotItem
                objectName: "hotspotItem"
                text: i18n.tr("Hotspot")
                enabled: Connectivity.hotspotStored
                control: Switch {
                    id: hotspotSwitch
                    objectName: "hotspotSwitch"
                    enabled: !switchSync.syncWaiting

                    USC.ServerPropertySynchroniser {
                        id: switchSync
                        userTarget: hotspotSwitch
                        userProperty: "checked"
                        serverTarget: Connectivity
                        serverProperty: "hotspotEnabled"
                        useWaitBuffer: true

                        // Since this blocks the UI thread, we wait until
                        // the UI has completed the checkbox animation before we
                        // ask the server to uipdate.
                        onSyncTriggered: {
                            triggerTimer.value = value;
                            triggerTimer.start();
                        }
                    }

                    // Catch taps if Wi-Fi is disable and prompt user.
                    MouseArea {
                        id: hotspotSwitchWhenWifiDisabled
                        anchors.fill: parent
                        visible: false
                        onClicked: enableWifiAction.diag = PopupUtils.open(
                            enableWifiDialog
                        );
                    }
                }
            }

            ListItem.Caption {
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: units.gu(2)
                    rightMargin: units.gu(2)
                }
                text : Connectivity.hotspotStored ?
                  i18n.tr("When hotspot is on, other devices can use your cellular data connection over Wi-Fi. Normal data charges apply.")
                  : i18n.tr("Other devices can use your cellular data connection over the Wi-Fi network. Normal data charges apply.")
            }

            Button {
                id: hotspotSetupButton
                objectName: "hotspotSetupButton"
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - units.gu(4)
                text: Connectivity.hotspotStored ?
                    i18n.tr("Change password/setup…") : i18n.tr("Set up hotspot…")

                onClicked: {
                    setup.setSource(Qt.resolvedUrl("HotspotSetup.qml"));
                    PopupUtils.open(setup.item, root, {});
                }
            }
        }
    }

    Timer {
        id: triggerTimer
        property bool value
        interval: 250; repeat: false
        onTriggered: Connectivity.hotspotEnabled = value
    }


    Action {
        id: enableWifiAction
        property var diag
        onTriggered: {
            // As soon as Wi-Fi has been turned on, activate the USC
            // synchroniser.
            function wifiUpdated (updated) {
                Connectivity.wifiEnabledUpdated.disconnect(wifiUpdated);
                switchSync.activate();
                PopupUtils.close(diag);
            }

            if (!Connectivity.wifiEnabled) {
                Connectivity.wifiEnabledUpdated.connect(wifiUpdated);
                hotspotSwitch.checked = true;
                Connectivity.setwifiEnabled(true);
            }
        }
    }

    Component {
        id: enableWifiDialog
        Dialog {
            id: dialog
            objectName: "enableWifiDialog"
            title: i18n.tr("Wi-Fi is off")
            text: i18n.tr("In order to create a hotspot, you need to turn Wi-Fi on.")
            visible: showAllUI

            Button {
                text: i18n.tr("Cancel")
                onClicked: PopupUtils.close(dialog)
            }

            Button {
                objectName: "confirmEnable"
                text: i18n.tr("Turn on Wi-Fi")
                onClicked: enableWifiAction.trigger()
            }
        }
    }
}
