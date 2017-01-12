/*
 * Copyright 2016 Canonical Ltd.
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

import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.Components.Popups 1.3
import Ubuntu.SystemSettings.Brightness 1.0

ItemPage {
    id: wifiDisplays
    objectName: "wifiDisplays"
    title: i18n.tr("Wireless Display")
    flickable: pageFlickable

    Component.onCompleted: {
        if (!displays.scanning)
            displays.scan();
    }

    property string selectedDisplayName

    function showError(error, displayName) {
        var diag = PopupUtils.open(errorAlert, null, {"error": error, "displayName": selectedDisplayName});
    }

    Component {
        id: errorAlert
        WifiDisplaysAlert {}
    }

    // Aethercast only caches results of devices discovered during scan
    // for a limited time, so we'll automatically rescan 60s after scanning
    // finishes
    Timer {
        id: scanTimer
        interval: 60000
        repeat: true
        running: !displays.scanning
        onTriggered: {
            if (!displays.scanning && displays.state !== AethercastDevice.Connected) {
                displays.scan();
            }
        }
    }

    Flickable {
        id: pageFlickable
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height

        // Only allow flicking if the content doesn't fit on the page
        boundsBehavior: (contentHeight > wifiDisplays.height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds

        AethercastDisplays {
            id: displays

            onConnectError: {
                console.error("onConnectError: " + error);
                showError(error);
            }

            // Log some info to help debug
            onConnectedDevicesChanged: {
                console.warn("ConnectedDevices: " + displays.connectedDevices.count);
            }

            // Log some info to help debug
            onDisconnectedDevicesChanged: {
                console.warn("DisconnectedDevices: " + displays.disconnectedDevices.count);
            }
        }

        Label {
            anchors {
                left: parent.left
                right: parent.right
            }
            visible: displays.devices.count < 1
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            height: units.gu(8)
            text: i18n.tr("No displays detected")
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            visible: displays.devices.count > 0

            ListItem.Standard {
                objectName: "connectedDisplay"
                text: i18n.tr("Connected display:")
                visible: displays.connectedDevices.count > 0
            }

            Repeater {
                model: displays.connectedDevices ? displays.connectedDevices : null
                delegate: ListItem.Subtitled {
                    id: displayDelegate
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    iconName: "video-display"
                    iconFrame: false
                    text: displayName
                    subText: stateName === AethercastDevice.Connected ? i18n.tr("Connected") : (stateName === AethercastDevice.Association || stateName == AethercastDevice.Connected ? i18n.tr("Connecting") : i18n.tr("Not connected"))
                    enabled: stateName === AethercastDevice.Idle || stateName === AethercastDevice.Connected || stateName === AethercastDevice.Disconnected
                    onClicked: {
                        if (stateName === AethercastDevice.Connected)
                            displays.disconnectDevice(addressName);
                        else {
                            selectedDisplayName = displayName;
                            displays.connectDevice(addressName);
                        }
                    }
                }
            }

            ListItem.Divider {
                visible: displays.disconnectedDevices.count > 0
            }

            ListItem.Standard {
                text: (displays.state === "connected" && displays.disconnectedDevices.count > 0) ? i18n.tr("Use another display:") : i18n.tr("Choose a display:")
                visible: displays.disconnectedDevices.count > 0
                control: Item {
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height
                    width: childrenRect.width
                    ActivityIndicator {
                        id: ind
                        anchors.centerIn: parent
                        running: displays.scanning
                        visible: running
                    }
                }
            }

            Repeater {
                model: displays.disconnectedDevices ? displays.disconnectedDevices : null
                delegate: ListItem.Subtitled {
                    id: displayDelegate
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    iconName: "video-display"
                    iconFrame: false
                    text: displayName
                    subText: (stateName === AethercastDevice.Connected) ? i18n.tr("Connected") : i18n.tr("Not connected")
                    enabled: stateName === AethercastDevice.Idle || stateName === AethercastDevice.Connected || stateName === AethercastDevice.Disconnected
                    onClicked: {
                        if (stateName === AethercastDevice.Connected)
                            displays.disconnectDevice(addressName);
                        else {
                            selectedDisplayName = displayName;
                            displays.connectDevice(addressName);
                        }
                    }
                }
            }
        }
    }
}
