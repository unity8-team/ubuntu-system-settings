/*
 * This file is part of ubuntu-system-settings
 *
 * Copyright (C) 2013-2015 Canonical Ltd.
 *
 * Contact: Charles Kerr <charles.kerr@canonical.com>
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

import QMenuModel 0.1
import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.SystemSettings.Bluetooth 1.0

Page {
    id: connectedDevicePage

    property Item root: null
    property UbuntuBluetoothPanel backend: null

    function getStatusString(connection) {
      switch (connection) {
      case Device.Connected:     return i18n.tr("Connected");
      case Device.Connecting:    return i18n.tr("Connecting…");
      case Device.Disconnecting: return i18n.tr("Disconnecting…");
      case Device.Disconnected:  return i18n.tr("Disconnected");
      default:                   return i18n.tr("Unknown");
      }
    }

    function getTypeString(type) {
        switch (type) {
        case Device.Computer:   return i18n.tr("Computer");
        case Device.Phone:      return i18n.tr("Phone");
        case Device.Modem:      return i18n.tr("Modem");
        case Device.Network:    return i18n.tr("Network");
        case Device.Headset:    return i18n.tr("Headset");
        case Device.Headphones: return i18n.tr("Headphones");
        case Device.Video:      return i18n.tr("Video");
        case Device.OtherAudio: return i18n.tr("Other Audio");
        case Device.Joypad:     return i18n.tr("Joypad");
        case Device.Keyboard:   return i18n.tr("Keyboard");
        case Device.Tablet:     return i18n.tr("Tablet");
        case Device.Mouse:      return i18n.tr("Mouse");
        case Device.Printer:    return i18n.tr("Printer");
        case Device.Camera:     return i18n.tr("Camera");
        case Device.Watch:      return i18n.tr("Watch");
        default:                return i18n.tr("Other");
        }
    }

    function getSignalString(strength) {
        switch (strength) {
        case Device.Excellent: return i18n.tr("Excellent");
        case Device.Good:      return i18n.tr("Good");
        case Device.Fair:      return i18n.tr("Fair");
        case Device.Poor:      return i18n.tr("Poor");
        default:               return i18n.tr("None");
        }
    }

    title: backend.selectedDevice ?
              backend.selectedDevice.name.length > 0 ?
                 backend.selectedDevice.name :
                 backend.selectedDevice.address
              : i18n.tr("None")
    visible: false

    Flickable {
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }

            ListItem.SingleValue {
                text: i18n.tr("Name")
                value: backend.selectedDevice &&
                       backend.selectedDevice.name.length > 0 ?
                             backend.selectedDevice.name :
                             i18n.tr("None")
            }
            ListItem.Standard {
                Rectangle {
                    color: "transparent"
                    anchors.fill: parent
                    anchors.topMargin: units.gu(1)
                    anchors.leftMargin: units.gu(2)
                    anchors.rightMargin: units.gu(2)

                    Label {
                        anchors {
                            top: parent.top
                            left: parent.left
                            topMargin: units.gu(1)
                        }
                        height: units.gu(3)
                        text: i18n.tr("Type")
                    }
                    Image {
                        anchors {
                            right: deviceType.left
                            rightMargin: units.gu(1)
                        }
                        height: units.gu(4)
                        width: units.gu(4)
                        source: backend.selectedDevice ? backend.selectedDevice.iconName : ""
                    }
                    Label {
                        id: deviceType
                        anchors {
                            top: parent.top
                            right: parent.right
                            topMargin: units.gu(1)
                        }
                        height: units.gu(3)
                        text: getTypeString(backend.selectedDevice ? backend.selectedDevice.type : Device.OTHER)
                    }
                }
            }
            ListItem.SingleValue {
                text: i18n.tr("Status")
                value: getStatusString(backend.selectedDevice ? backend.selectedDevice.connection : Device.Disconnected)
            }
            ListItem.SingleValue {
                text: i18n.tr("Signal Strength")
                value: getSignalString(backend.selectedDevice ? backend.selectedDevice.strength : Device.None)
            }
            ListItem.Standard {
                id: trustedCheck
                text: i18n.tr("Connect automatically when detected:")
                visible: backend.selectedDevice.paired
                control: CheckBox {
                    property bool serverChecked: backend.selectedDevice ? backend.selectedDevice.trusted : false
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: {
                        if (backend.selectedDevice) {
                            backend.selectedDevice.trusted = checked;
                        }
                    }
                }
            }
            ListItem.SingleControl {
                control: Button {
                    text: backend.selectedDevice && (backend.selectedDevice.connection == Device.Connected || backend.selectedDevice.connection == Device.Connecting) ? i18n.tr("Disconnect") : i18n.tr("Connect")
                    width: parent.width - units.gu(8)
                    onClicked: {
                        if (backend.selectedDevice
                            && (backend.selectedDevice.connection == Device.Connected
                                || backend.selectedDevice.connection == Device.Connecting)) {
                            backend.disconnectDevice();
                        } else {
                            backend.stopDiscovery()
                            backend.connectDevice(backend.selectedDevice.address);
                        }

                        backend.resetSelectedDevice();
                        pageStack.pop();
                    }
                    visible: backend.selectedDevice ? true : false
                    enabled: backend.selectedDevice && backend.powered ? true : false
                }
            }
            ListItem.SingleControl {
                control: Button {
                    text: i18n.tr("Forget this device")
                    width: parent.width - units.gu(8)
                    onClicked: {
                        backend.removeDevice();
                        backend.resetSelectedDevice();
                        pageStack.pop();
                    }
                    enabled: backend.powered && backend.selectedDevice && backend.selectedDevice.path.length > 0 && backend.selectedDevice.paired ? true : false
                }
            }
        }
    }
}
