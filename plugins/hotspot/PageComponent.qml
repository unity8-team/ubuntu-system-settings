/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014-2016 Canonical Ltd.
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

import QtQuick 2.4
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Connectivity 1.0

ItemPage {

    id: root
    objectName: "hotspotPage"
    title: i18n.tr("Hotspot")
    flickable: flick

    states: [
        State {
            name: "disabled"
            // Undefined WifiEnabled means Connectivity is unavailable.
            when: typeof Connectivity.wifiEnabled === "undefined" ||
                  Connectivity.flightMode
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
            when: Connectivity.wifiEnabled === false
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
            spacing: units.gu(1)

            SettingsListItems.Standard {
                id: hotspotItem
                objectName: "hotspotItem"
                text: i18n.tr("Hotspot")
                enabled: Connectivity.hotspotStored
                onClicked: hotspotSwitch.trigger()

                Switch {
                    id: hotspotSwitch
                    objectName: "hotspotSwitch"
                    enabled: parent.enabled
                    checked: Connectivity.hotspotEnabled
                    onTriggered: Connectivity.hotspotEnabled = checked

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

            ListItems.Caption {
                anchors {
                    left: parent.left
                    right: parent.right
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
                    i18n.tr("Change Password/Setup…") : i18n.tr("Set Up Hotspot…")
                onClicked: {
                    setup.setSource(Qt.resolvedUrl("HotspotSetup.qml"));
                    PopupUtils.open(setup.item, root, {});
                }
            }
        }
    }

    Action {
        id: enableWifiAction
        property var diag
        onTriggered: {
            // As soon as Wi-Fi has been turned on, enable the hotspot.
            function wifiUpdated (updated) {
                Connectivity.wifiEnabledUpdated.disconnect(wifiUpdated);
                Connectivity.hotspotEnabled = true;
                PopupUtils.close(diag);
            }

            Connectivity.wifiEnabledUpdated.connect(wifiUpdated);
            hotspotSwitch.checked = true;
            Connectivity.wifiEnabled = true;
        }
    }

    Component {
        id: enableWifiDialog
        Dialog {
            id: dialogue
            objectName: "enableWifiDialog"
            title: i18n.tr("Wi-Fi is off")
            text: i18n.tr("In order to create a hotspot, you need to turn Wi-Fi on.")

            Button {
                text: i18n.tr("Cancel")
                onClicked: PopupUtils.close(dialogue)
            }

            Button {
                objectName: "confirmEnable"
                text: i18n.tr("Turn on Wi-Fi")
                onClicked: enableWifiAction.trigger()
            }
        }
    }

    Connections {
        target: Connectivity
        onHotspotEnabledUpdated: hotspotSwitch.checked = target.hotspotEnabled
    }
}
