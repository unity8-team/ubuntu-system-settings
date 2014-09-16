/*
 * Copyright 2013 Canonical Ltd.
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

import QtQuick 2.0
import QtQuick.Layouts 1.1
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import Ubuntu.SystemSettings.Wifi 1.0
import QMenuModel 0.1

Component {

    Dialog {

        Common {
            id: common
        }

        states: [
            State {
                name: "CONNECTING"
                PropertyChanges {
                    target: connectButton
                    enabled: false
                }
                PropertyChanges {
                    target: connectButtonIndicator
                    running: true
                }
                PropertyChanges {
                    target: passwordVisible
                    enabled: false
                }
                PropertyChanges {
                    target: password
                    enabled: false
                }
                PropertyChanges {
                    target: passwordList
                    enabled: false
                }
                PropertyChanges {
                    target: securityList
                    enabled: false
                }
                PropertyChanges {
                    target: feedback
                    opacity: 0.5
                }
                PropertyChanges {
                    target: networkname
                    enabled: false
                }
                PropertyChanges {
                    target: networknameLabel
                    opacity: 0.5
                }
            },
            State {
                name: "FAILED"
            },
            State {
                name: "SUCCEEDED"
                PropertyChanges {
                    target: successIndicator
                    running: true
                }
                PropertyChanges {
                    target: cancelButton
                    enabled: false
                }
                PropertyChanges {
                    target: connectButton
                    enabled: false
                }
            }
        ]

        id: otherNetworkDialog
        objectName: "otherNetworkDialog"
        function settingsValid() {
            if(networkname.length == 0) {
                return false;
            }
            if(securityList.selectedIndex == 0) {
                return true
            }
            if(securityList.selectedIndex == 1) {
                return password.length >= 8
            }
            // WEP
            return password.length === 5  ||
                   password.length === 10 ||
                   password.length === 13 ||
                   password.length === 26;
        }

        title: i18n.tr("Connect to Hidden Network")

        Label {
            id: feedback
            horizontalAlignment: Text.AlignHCenter
            height: contentHeight
            wrapMode: Text.Wrap
        }

        ListItem.Standard {
            id: networknameLabel
            text : i18n.tr("Network name")
            showDivider: false
            highlightWhenPressed: false
            height: units.gu(2)
        }

        TextField {
            id : networkname
            objectName: "networkname"
            inputMethodHints: Qt.ImhNoPredictiveText
        }

        ListItem.ItemSelector {
            id: securityList
            objectName: "securityList"
            text: i18n.tr("Security")
            model: [i18n.tr("None"),                 // index: 0
                    i18n.tr("WPA & WPA2 Personal"),  // index: 1
                    i18n.tr("WEP"),                  // index: 2
                    ]
        }

        ListItem.Standard {
            id: passwordList
            visible: securityList.selectedIndex !== 0
            text: i18n.tr("Password")
            control : TextInput {
            }
            showDivider: false
        }

        TextField {
            id : password
            objectName: "password"
            visible: securityList.selectedIndex !== 0
            echoMode: passwordVisibleSwitch.checked ? TextInput.Normal : TextInput.Password
        }

        ListItem.Standard {
            text: i18n.tr("Password visible")
            visible: securityList.selectedIndex !== 0
            id: passwordVisible
            control: Switch {
                id: passwordVisibleSwitch
            }
        }

        RowLayout {
            id: buttonRow
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(2)
            height: cancelButton.height

            Button {
                id: cancelButton
                objectName: "cancel"
                Layout.fillWidth: true
                text: i18n.tr("Cancel")
                onClicked: {
                    PopupUtils.close(otherNetworkDialog);

                    // If this dialog created the connection,
                    // disconnect the device
                    if (otherNetworkDialog.state === "CONNECTING") {
                        DbusHelper.disconnectDevice();
                    }
                }
            }

            Button {
                id: connectButton
                objectName: "connect"
                Layout.fillWidth: true
                text: i18n.tr("Connect")
                enabled: settingsValid()
                onClicked: {
                    DbusHelper.connect(
                        networkname.text,
                        securityList.selectedIndex,
                        password.text)
                    otherNetworkDialog.state = "CONNECTING";
                }
                Icon {
                    height: parent.height - units.gu(1.5)
                    width: parent.height - units.gu(1.5)
                    anchors {
                        centerIn: parent
                    }
                    name: "tick"
                    color: "green"
                    visible: successIndicator.running
                }
                ActivityIndicator {
                    id: connectButtonIndicator
                    running: false
                    visible: running
                    height: parent.height - units.gu(1.5)
                    anchors {
                        centerIn: parent
                    }
                }
            }
        }

        Timer {
            id: successIndicator
            interval: 2000
            running: false
            repeat: false
            onTriggered: PopupUtils.close(otherNetworkDialog)
        }

        Connections {
            target: DbusHelper
            onDeviceStateChanged: {

                if (otherNetworkDialog.state === "FAILED") {
                    // Disconnect the device if it tries to reconnect after a
                    // connection failure
                    if (newState === 40) { // 40 = NM_DEVICE_STATE_PREPARE
                        DbusHelper.disconnectDevice();
                    }
                }

                switch (newState) {
                    case 120:
                        // connection failed only if we created it
                        if (otherNetworkDialog.state === "CONNECTING") {
                            otherNetworkDialog.state = "FAILED";
                            console.warn('otherNetworkDialog.state',
                                otherNetworkDialog.state);
                            feedback.text = common.reasonToString(reason);
                        }
                        break;
                    case 100:
                        // connection succeeded only if it was us that
                        // created it
                        if (otherNetworkDialog.state === "CONNECTING") {
                            otherNetworkDialog.state = "SUCCEEDED";
                        }
                        break;
                }

            }
        }
    }
}

