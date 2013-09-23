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

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Bluetooth 1.0


PageStack {

    Component {
        id: confirmPasskeyDialog
        ConfirmPasskeyDialog { }
    }

    Component {
        id: providePasskeyDialog
        ProvidePasskeyDialog { }
    }

    id: pageStack
    Component.onCompleted: push(mainPage)
    UbuntuBluetoothPanel { id: backend }

    Connections {
        target: backend.agent
        onPasskeyConfirmationNeeded: {
            var popup = PopupUtils.open (confirmPasskeyDialog, pageStack, {passkey: passkey, name: device.name})
            popup.canceled.connect (function() {target.confirmPasskey (tag, false)})
            popup.confirmed.connect (function() {target.confirmPasskey (tag, true)})
        }
        onPasskeyNeeded: {
            var popup = PopupUtils.open (providePasskeyDialog, pageStack, {name: device.name})
            popup.canceled.connect (function() {target.providePasskey (tag, false, 0)})
            popup.provided.connect (function(passkey) {target.providePasskey (tag, true, passkey)})
        }
    }

    function getDisplayName (connection, displayName) {
      if (connection == Device.Connecting)
        return i18n.tr("%1 (Connecting…)").arg(displayName);
      else if (connection == Device.Disconnecting)
        return i18n.tr("%1 (Disconnecting…)").arg(displayName);
      else
        return displayName;
    }

    function getTypeString (type) {
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

    function getSignalString (strength) {
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

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Standard {
                text: i18n.tr ("Bluetooth")
                control: Switch {
                    id: enabledControl
                    checked: backend.enabled
                    onClicked: backend.enabled = checked
                }
            }

            //  Connnected Headset(s)

            ListItem.Standard {
                id: connectedHeader
                text: i18n.tr("Connected headset:")

                enabled: backend.enabled
                visible: connectedList.visible
            }
            ListView {
                id: connectedList
                width: parent.width
                height: connectedHeader.height * count

                enabled: backend.enabled
                visible: backend.enabled && (count > 0)

                model: backend.connectedHeadsets
                delegate: ListItem.Standard {
                    icon: iconName
                    text: getDisplayName (connection, displayName)
                    onClicked: {
                        backend.setSelectedDevice (addressName);
                        pageStack.push(connectedHeadsetPage);
                    }
                }
            }

            //  Disconnnected Headset(s)

            ListItem.Standard {
                id: disconnectedHeader
                text: connectedList.visible ? i18n.tr ("Connect a different headset:") : i18n.tr ("Connect a headset:")

                enabled: backend.enabled
            }
            ListView {
                id: disconnectedList
                width: parent.width
                height: disconnectedHeader.height * count

                enabled: backend.enabled
                visible: backend.enabled && (count > 0)

                model: backend.disconnectedHeadsets
                delegate: ListItem.Standard {
                    icon: iconName
                    text: getDisplayName (connection, displayName)
                    onClicked: {
                        backend.connectHeadset (addressName);
                    }
                }
            }
            ListItem.Standard {
                id: disconnectedNone
                text: i18n.tr("None detected")
                enabled: backend.enabled
                visible: !disconnectedList.visible
            }
        }
    }

    Page {
        id: connectedHeadsetPage
        visible: false
        title: backend.selectedDevice ? backend.selectedDevice.name : i18n.tr("None")

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.SingleValue {
                text: i18n.tr("Name")
                value: backend.selectedDevice ? backend.selectedDevice.name : i18n.tr("None")
            }
            ListItem.SingleValue {
                text: i18n.tr("Type")
                value: getTypeString (backend.selectedDevice ? backend.selectedDevice.type : Device.OTHER)
            }
            ListItem.SingleValue {
                text: i18n.tr("Signal Strength")
                value: getSignalString (backend.selectedDevice ? backend.selectedDevice.strength : Device.None)
            }
            ListItem.SingleControl {
                control: Button {
                    text: i18n.tr("Disconnect")
                    width: parent.width - units.gu(8)
                    onClicked: {
                        backend.disconnectHeadset ();
                        pageStack.pop();
                    }
                }
            }
        }
    }
}
