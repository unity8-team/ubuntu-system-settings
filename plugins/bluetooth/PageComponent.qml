/*
 * This file is part of ubuntu-system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
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
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Bluetooth 1.0


ItemPage {
    id: root
    title: i18n.tr("Bluetooth")

    UbuntuBluetoothPanel { id: backend }

    Component {
        id: confirmPasskeyDialog
        ConfirmPasskeyDialog { }
    }

    Component {
        id: providePasskeyDialog
        ProvidePasskeyDialog { }
    }

    Component {
        id: providePinCodeDialog
        ProvidePinCodeDialog { }
    }

    Connections {
        target: backend.agent
        onPasskeyConfirmationNeeded: {
            var request_tag = tag
            var popup = PopupUtils.open(confirmPasskeyDialog, root, {passkey: passkey, name: device.name})
            popup.canceled.connect(function() {target.confirmPasskey(request_tag, false)})
            popup.confirmed.connect(function() {target.confirmPasskey(request_tag, true)})
        }
        onPasskeyNeeded: {
            var request_tag = tag
            var popup = PopupUtils.open(providePasskeyDialog, root, {name: device.name})
            popup.canceled.connect(function() {target.providePasskey(request_tag, false, 0)})
            popup.provided.connect(function(passkey) {target.providePasskey(request_tag, true, passkey)})
        }
        onPinCodeNeeded: {
            var request_tag = tag
            var popup = PopupUtils.open(providePinCodeDialog, root, {name: device.name})
            popup.canceled.connect(function() {target.providePinCode(request_tag, false, "")})
            popup.provided.connect(function(pinCode) {target.providePinCode(request_tag, true, pinCode)})
        }
    }

    function getDisplayName(type, displayName) {
        /* TODO: If the device requires a PIN, show ellipsis.
         * Right now we don't have a way to check this, just return the name
         */
        return displayName;
    }

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
            anchors.left: parent.left
            anchors.right: parent.right

            QDBusActionGroup {
                id: bluetoothActionGroup
                busType: DBus.SessionBus
                busName: "com.canonical.indicator.bluetooth"
                objectPath: "/com/canonical/indicator/bluetooth"

                property variant enabled: action("bluetooth-enabled")

                Component.onCompleted: start()
            }

            ListItem.Standard {
                text: i18n.tr("Bluetooth")
                control: Switch {
                    id: btSwitch
                    // Cannot use onCheckedChanged as this triggers a loop
                    onClicked: bluetoothActionGroup.enabled.activate()
                    checked: backend.powered
                }
                Component.onCompleted: clicked.connect(btSwitch.clicked)
            }

            Binding {
                target: btSwitch
                property: "checked"
                value: bluetoothActionGroup.enabled.state
            }

            // Discoverability
            ListItem.Standard {
                enabled: bluetoothActionGroup.enabled
                showDivider: false

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
                        text: backend.discoverable ? i18n.tr("Discoverable") : i18n.tr("Not discoverable")
                    }

                    Label {
                        anchors {
                            top: parent.top
                            right: parent.right
                            topMargin: units.gu(1)
                        }
                        height: units.gu(3)
                        text: backend.discoverable ? backend.adapterName() : ""
                        color: "darkgrey"
                        visible: backend.discoverable
                        enabled: false
                    }

                    ActivityIndicator {
                        anchors {
                            top: parent.top
                            right: parent.right
                            topMargin: units.gu(1)
                        }
                        visible: backend.powered && !backend.discoverable
                        running: true
                    }
                }
            }

            //  Connnected Headset(s)
            ListItem.Standard {
                id: connectedHeader
                text: i18n.tr("Connected devices:")

                enabled: bluetoothActionGroup.enabled
                visible: connectedList.visible
            }

            Column {
                id: connectedList
                anchors {
                    left: parent.left
                    right: parent.right
                }
                visible: bluetoothActionGroup.enabled && (connectedRepeater.count > 0)

                Repeater {
                    id: connectedRepeater
                    model: backend.connectedDevices
                    delegate: ListItem.Standard {
                        iconSource: iconPath
                        iconFrame: false
                        text: getDisplayName(type, displayName)
                        control: ActivityIndicator {
                            visible: connection == Device.Connecting
                            running: true
                        }
                        onClicked: {
                            backend.setSelectedDevice(addressName);
                            pageStack.push(connectedDevicePage);
                        }
                        progression: true
                    }
                }
            }

            //  Disconnnected Headset(s)

            SettingsItemTitle {
                id: disconnectedHeader
                text: connectedList.visible ? i18n.tr("Connect another device:") : i18n.tr("Connect a device:")
                enabled: bluetoothActionGroup.enabled
                control: ActivityIndicator {
                    visible: backend.powered && backend.discovering
                    running: true
                }
            }

            Column {
                id: disconnectedList
                anchors {
                    left: parent.left
                    right: parent.right
                }
                visible: bluetoothActionGroup.enabled && (disconnectedRepeater.count > 0)

                Repeater {
                    id: disconnectedRepeater
                    model: backend.disconnectedDevices
                    delegate: ListItem.Standard {
                        iconSource: iconPath
                        iconFrame: false
                        text: getDisplayName(type, displayName)
                        enabled: backend.isSupportedType(type)
                        onClicked: {
                            backend.setSelectedDevice(addressName);
                            pageStack.push(connectedDevicePage);
                        }
                        progression: true
                    }
                }
            }
            ListItem.Standard {
                id: disconnectedNone
                text: i18n.tr("None detected")
                visible: !disconnectedList.visible
                enabled: false
            }

            //  Devices that connect automatically
            SettingsItemTitle {
                id: autoconnectHeader
                text: i18n.tr("Connect automatically when detected:")
                visible: autoconnectList.visible
                enabled: bluetoothActionGroup.enabled
            }
            Column {
                id: autoconnectList
                anchors {
                    left: parent.left
                    right: parent.right
                }

                visible: bluetoothActionGroup.enabled && (autoconnectRepeater.count > 0)

                Repeater {
                    id: autoconnectRepeater
                    model: backend.autoconnectDevices
                    delegate: ListItem.Standard {
                        iconSource: iconPath
                        iconFrame: false
                        text: getDisplayName(type, displayName)
                        onClicked: {
                            backend.setSelectedDevice(addressName);
                            pageStack.push(connectedDevicePage);
                        }
                        progression: true
                    }
                }
            }
        }
    }

    Page {
        id: connectedDevicePage
        title: backend.selectedDevice ? backend.selectedDevice.name : i18n.tr("None")
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
                    value: backend.selectedDevice ? backend.selectedDevice.name : i18n.tr("None")
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
                    control: CheckBox {
                        onClicked: {
                            if (backend.selectedDevice) {
                                backend.selectedDevice.trusted = !backend.selectedDevice.trusted
                            }
                        }
                        checked: backend.selectedDevice ? backend.selectedDevice.trusted : false
                    }
                    Component.onCompleted:
                        clicked.connect(trustedCheck.clicked)
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
                            pageStack.pop();
                        }
                        visible: backend.selectedDevice ? true : false
                    }
                }
                ListItem.SingleControl {
                    control: Button {
                        text: i18n.tr("Forget this device")
                        width: parent.width - units.gu(8)
                        onClicked: {
                            backend.removeDevice();
                            pageStack.pop();
                        }
                        enabled: backend.selectedDevice && backend.selectedDevice.path.length > 0 ? true : false
                    }
                }
            }
        }
    }
}
