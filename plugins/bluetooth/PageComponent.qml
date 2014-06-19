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
            var popup = PopupUtils.open(confirmPasskeyDialog, root, {passkey: passkey, name: device.name})
            popup.canceled.connect(function() {target.confirmPasskey(tag, false)})
            popup.confirmed.connect(function() {target.confirmPasskey(tag, true)})
        }
        onPasskeyNeeded: {
            var popup = PopupUtils.open(providePasskeyDialog, root, {name: device.name})
            popup.canceled.connect(function() {target.providePasskey(tag, false, 0)})
            popup.provided.connect(function(passkey) {target.providePasskey(tag, true, passkey)})
        }
        onPinCodeNeeded: {
            var popup = PopupUtils.open(providePinCodeDialog, root, {name: device.name})
            popup.canceled.connect(function() {target.providePinCode(tag, false, "")})
            popup.provided.connect(function(pinCode) {target.providePinCode(tag, true, pinCode)})
        }
    }

    function getDisplayName(connection, displayName) {
      if (connection == Device.Connecting)
        // TRANSLATORS: %1 is the display name of the device that is connecting
        return i18n.tr("%1 (Connecting…)").arg(displayName);
      else if (connection == Device.Disconnecting)
        // TRANSLATORS: %1 is the display name of the device that is disconnecting
        return i18n.tr("%1 (Disconnecting…)").arg(displayName);
      else
        return displayName;
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

    Page {
        id: mainPage
        title: i18n.tr("Bluetooth")
        visible: true
        anchors.fill: parent

        Column {
            anchors.fill: parent

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
                }
                Component.onCompleted: clicked.connect(btSwitch.clicked)
            }

            Binding {
                target: btSwitch
                property: "checked"
                value: bluetoothActionGroup.enabled.state
            }

            // Discoverability
            ListItem.Subtitled {
                text: backend.discoverable ? i18n.tr("Discoverable") : i18n.tr("Not discoverable")
                enabled: bluetoothActionGroup.enabled
                //Component.onCompleted: clicked.connect(btSwitch.clicked)
                //control: ActivityIndicator {
                //    visible: !backend.discoverable
                //    running: true
                //}
                // Until we can have control in a Subtitled ListItem:
                subText: backend.discoverable ? backend.getAdapterName() : ""
                progression: !backend.discoverable
            }

            //  Connnected Headset(s)
            ListItem.Standard {
                id: connectedHeader
                text: i18n.tr("Connected devices:")

                enabled: bluetoothActionGroup.enabled
                visible: connectedList.visible
            }

            ListView {
                id: connectedList
                width: parent.width
                height: connectedHeader.height * count

                visible: bluetoothActionGroup.enabled && (count > 0)

                model: backend.connectedDevices
                delegate: ListItem.Standard {
                    iconName: iconName
                    text: getDisplayName(connection, displayName)
                    onClicked: {
                        backend.setSelectedDevice(addressName);
                        pageStack.push(connectedDevicePage);
                    }
                }
            }

            //  Disconnnected Headset(s)

            ListItem.Standard {
                id: disconnectedHeader
                text: connectedList.visible ? i18n.tr("Connect a different device:") : i18n.tr("Connect another device:")
                enabled: bluetoothActionGroup.enabled
                control: ActivityIndicator {
                    visible: backend.discovering
                    running: true
                }
            }

            ListView {
                id: disconnectedList
                width: parent.width
                height: disconnectedHeader.height * count

                visible: bluetoothActionGroup.enabled && (count > 0)

                model: backend.disconnectedDevices
                delegate: ListItem.Standard {
                    iconName: iconName
                    text: getDisplayName(connection, displayName)
                    onClicked: {
                        backend.connectDevice(addressName);
                    }
                }
            }
            ListItem.Standard {
                id: disconnectedNone
                text: i18n.tr("None detected")
                visible: !disconnectedList.visible
                enabled: !backend.discovering
            }
        }
    }

    Page {
        id: connectedDevicePage
        title: backend.selectedDevice ? backend.selectedDevice.name : i18n.tr("None")
        visible: false

        Column {
            anchors.fill: parent

            ListItem.SingleValue {
                text: i18n.tr("Name")
                value: backend.selectedDevice ? backend.selectedDevice.name : i18n.tr("None")
            }
            ListItem.SingleValue {
                text: i18n.tr("Type")
                value: getTypeString(backend.selectedDevice ? backend.selectedDevice.type : Device.OTHER)
            }
            ListItem.SingleValue {
                text: i18n.tr("Signal Strength")
                value: getSignalString(backend.selectedDevice ? backend.selectedDevice.strength : Device.None)
            }
            ListItem.SingleControl {
                control: Button {
                    text: i18n.tr("Disconnect")
                    width: parent.width - units.gu(8)
                    onClicked: {
                        backend.disconnectDevice();
                        pageStack.pop();
                    }
                }
            }
        }
    }
}
