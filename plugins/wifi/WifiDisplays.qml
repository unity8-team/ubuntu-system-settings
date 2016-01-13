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
import Ubuntu.SystemSettings.Wifi 1.0

ItemPage {
    id: wifiDisplays
    objectName: "wifiDisplays"
    title: i18n.tr("Wi-Fi Displays")

    Flickable {
        id: pageFlickable
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height

        Displays {
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
                objectName: "wifiDisplays"
                text: i18n.tr("Wi-Fi Displays")
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
                    Icon {
                        anchors.centerIn: parent
                        height: units.gu(4)
                        width: height
                        visible: !ind.running
                        name: "reload"
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                displays.scan();
                            }
                        }
                    }
                }
            }

            Repeater {
                id: mainMenu
                model: displays.devices ? displays.devices : null
                delegate: ListItem.Subtitled {
                    id: displayDelegate
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    text: displayName
                    subText: stateName
                    Button {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        height: units.gu(4)
                        visible: stateName !== "connecting"
                        text: stateName === "connected" ? i18n.tr("Disconnect") : i18n.tr("Connect")
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

        // Only allow flicking if the content doesn't fit on the page
        boundsBehavior: (contentHeight > wifiDisplays.height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
    }
}
