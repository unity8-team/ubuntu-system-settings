/*
 * This file is part of ubuntu-system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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
import QtQuick 2.4
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.SystemSettings.Bluetooth 1.0
import Ubuntu.Settings.Components 0.1 as USC


ItemPage {
    id: root
    title: i18n.tr("Bluetooth")
    flickable: scrollWidget
    objectName: "bluetoothPage"

    property var dialogPopupId
    property var currentDevice

    function finishDevicePairing() {
        if (root.dialogPopupId)
            PopupUtils.close(root.dialogPopupId)

        root.dialogPopupId = null
        root.currentDevice = null
    }

    UbuntuBluetoothPanel {
        id: backend

        onDevicePairingDone: {
            console.log("Got pairing status notification for device " + device.address)

            if (device != root.currentDevice)
                return

            finishDevicePairing()
        }
    }

    Timer {
        id: discoverableTimer
        repeat: false
        running: false
        onTriggered: backend.trySetDiscoverable(true)
    }

    property int lastApplicationState: Qt.ApplicationSuspended

    /* Disable BT visiblity/discovery when switching out */
    Connections {
        target: Qt.application
        onStateChanged: {
            if (Qt.application.state !== Qt.ApplicationActive) {
                backend.trySetDiscoverable(false)

                // We only increase the block count when we get inactive
                // and not for any other state as would then end up in
                // a state we can never escape from.
                if (Qt.application.state === Qt.ApplicationInactive &&
                    lastApplicationState === Qt.ApplicationActive)
                    backend.blockDiscovery()
            }
            else {
                discoverableTimer.start()
                backend.unblockDiscovery()
            }

            lastApplicationState = Qt.application.state
        }
    }

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

    Component {
       id: displayPinCodeDialog
       DisplayPinCodeDialog { }
    }

    Component {
        id: displayPasskeyDialog
        DisplayPasskeyDialog { }
    }

    Component {
        id: authorizationRequestDialog
        AuthorizationRequestDialog { }
    }

    Connections {
        target: backend.agent
        onCancelNeeded: finishDevicePairing()
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
        onDisplayPinCodeNeeded: {
            if (!root.dialogPopupId)
            {
                root.currentDevice = device
                root.dialogPopupId  = PopupUtils.open(displayPinCodeDialog, root, {pincode: pincode, name: device.name})
                root.dialogPopupId.canceled.connect(function() {
                    root.dialogPopupId = null
                    if (root.currentDevice) {
                        root.currentDevice.cancelPairing()
                        root.currentDevice = null
                    }
                })
            }
            else
            {
                console.warn("Unhandled PIN code request for device " + device.name);
            }
        }
        onDisplayPasskeyNeeded: {
            if (!root.dialogPopupId)
            {
                root.currentDevice = device
                root.dialogPopupId  = PopupUtils.open(displayPasskeyDialog, root, {passkey: passkey, name: device.name,
                                                 entered: entered})
                root.dialogPopupId.canceled.connect(function() {
                    root.dialogPopupId = null
                    if (root.currentDevice) {
                        root.currentDevice.cancelPairing()
                        root.currentDevice = null
                    }
                })
            }
            else
            {
                root.dialogPopupId.entered = entered
            }
        }
        onReleaseNeeded: {
            finishDevicePairing()
        }
        onAuthorizationRequested: {
            if (!root.dialogPopupId)
            {
                var request_tag = tag
                root.dialogPopupId  = PopupUtils.open(authorizationRequestDialog, root, {name: device.name})
                root.dialogPopupId.accepted.connect(function() {
                    root.dialogPopupId = null
                    target.authorizationRequestCallback(request_tag, true)
                })
                root.dialogPopupId.declined.connect(function() {
                    root.dialogPopupId = null
                    target.authorizationRequestCallback(request_tag, false)
                })
            }
        }
    }

    function getDisplayName(type, displayName) {
        /* TODO: If the device requires a PIN, show ellipsis.
         * Right now we don't have a way to check this, just return the name
         */
        return displayName;
    }

    Flickable {
        id: scrollWidget
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

            SettingsListItems.Standard {
                text: i18n.tr("Bluetooth")
                Switch {
                    id: btSwitch
                    property bool serverChecked: bluetoothActionGroup.enabled.state != undefined ? bluetoothActionGroup.enabled.state : false
                    USC.ServerPropertySynchroniser {
                        userTarget: btSwitch
                        userProperty: "checked"
                        serverTarget: btSwitch
                        serverProperty: "serverChecked"

                        onSyncTriggered: bluetoothActionGroup.enabled.activate()
                    }
                }
            }

            // Discoverability
            SettingsListItems.SingleValue {
                text: backend.discoverable ? i18n.tr("Discoverable") : i18n.tr("Not discoverable")
                value: backend.discoverable ? backend.adapterName() : ""
                enabled: bluetoothActionGroup.enabled
                showDivider: false

                Label {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        rightMargin: units.gu(2)
                    }
                    color: "darkgrey"
                    visible: backend.powered && !backend.discoverable
                    text: i18n.tr("Searching…")
                    fontSize: "x-small"
                    SlotsLayout.position: SlotsLayout.Trailing
                    SlotsLayout.overrideVerticalPositioning: true
                }
            }

            SettingsItemTitle {
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
                visible: (bluetoothActionGroup.enabled.state != undefined && bluetoothActionGroup.enabled.state) && (connectedRepeater.count > 0)
                objectName: "connectedList"

                Repeater {
                    id: connectedRepeater
                    model: backend.connectedDevices
                    delegate: SettingsListItems.IconProgression {
                        iconSource: iconPath
                        text: getDisplayName(type, displayName)
                        layout.subtitle.text: connection == Device.Connecting ? i18n.tr("Connecting…") : i18n.tr("Connected")
                        layout.subtitle.color: UbuntuColors.green

                        ActivityIndicator {
                            visible: connection == Device.Connecting
                            running: visible
                        }
                        onClicked: {
                            backend.setSelectedDevice(addressName);
                            pageStack.addPageToNextColumn(root,
                                Qt.resolvedUrl("DevicePage.qml"),
                                {backend: backend, root: root});
                        }
                    }
                }
            }

            SettingsItemTitle {
                id: disconnectedHeader
                text: connectedList.visible ? i18n.tr("Connect another device:") : i18n.tr("Connect a device:")
                enabled: bluetoothActionGroup.enabled.state != undefined ? bluetoothActionGroup.enabled.state : false
                Label {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        rightMargin: units.gu(2)
                    }
                    color: "darkgrey"
                    visible: backend.powered && backend.discovering
                    text: i18n.tr("Searching…")
                    fontSize: "x-small"
                    SlotsLayout.position: SlotsLayout.Trailing
                    SlotsLayout.overrideVerticalPositioning: true
                }
            }

            Column {
                id: disconnectedList
                anchors {
                    left: parent.left
                    right: parent.right
                }
                visible: (bluetoothActionGroup.enabled.state != undefined && bluetoothActionGroup.enabled.state) && (disconnectedRepeater.count > 0)
                objectName: "disconnectedList"

                Repeater {
                    id: disconnectedRepeater
                    model: backend.disconnectedDevices
                    delegate: SettingsListItems.IconProgression {
                        iconSource: iconPath
                        text: getDisplayName(type, displayName)
                        onClicked: {
                            backend.setSelectedDevice(addressName);
                            pageStack.addPageToNextColumn(root,
                                Qt.resolvedUrl("DevicePage.qml"),
                                {backend: backend, root: root});
                        }
                    }
                }
            }
            SettingsListItems.Standard {
                id: disconnectedNone
                text: i18n.tr("None detected")
                visible: !disconnectedList.visible && disconnectedHeader.visible
            }

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
                    delegate: SettingsListItems.IconProgression {
                        iconSource: iconPath
                        text: getDisplayName(type, displayName)
                        onClicked: {
                            backend.setSelectedDevice(addressName);
                            pageStack.addPageToNextColumn(root,
                                Qt.resolvedUrl("DevicePage.qml"),
                                {backend: backend, root: root});
                        }
                    }
                }
            }
        }
    }
}
