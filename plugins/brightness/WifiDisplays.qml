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
import Ubuntu.SystemSettings.Brightness 1.0

ItemPage {
    id: wifiDisplays
    objectName: "wifiDisplays"
    title: i18n.tr("Wireless Display")

    Component.onCompleted: {
        if (!displays.scanning)
            displays.scan();
    }

    Timer {
        id: scanTimer
        interval: 60000
        repeat: true
        running: !displays.scanning
        onTriggered: {
            console.warn("Timer triggered");
            if (!displays.scanning && displays.state !== "connected") {
                console.warn("Initiating a scan");
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
            onDevicesChanged: {
                console.warn("onDevicesChanged: " + devices);
            }
            onScanningChanged: {
                console.warn("onScanningChanged: " + scanning);
            }
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            ListItem.Standard {
                objectName: "connectedDisplay"
                text: i18n.tr("Connected display:")
                visible: displays.connectedDevices.length > 0
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
                    subText: stateName
                    enabled: stateName === "idle" || stateName === "connected" || stateName === "disconnected"
                    onClicked: {
                        if (stateName === "connected")
                            displays.disconnectDevice(addressName);
                        else
                            displays.connectDevice(addressName);
                    }
                }
            }

            ListItem.Divider {
                visible: displays.disconnectedDevices.length > 0
            }

            ListItem.Standard {
                objectName: "wifiDisplays"
                text: i18n.tr("Use another display:")
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
                    subText: stateName
                    enabled: stateName === "idle" || stateName === "connected" || stateName === "disconnected"
                    onClicked: {
                        if (stateName === "connected")
                            displays.disconnectDevice(addressName);
                        else
                            displays.connectDevice(addressName);
                    }
                }
            }

        }
    }
}
