/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
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
import QtQuick.Layouts 1.1
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Cellular 1.0
import Ubuntu.Components.Popups 0.1


Component {
    id: hotspotSetup

    Dialog {
        id: hotspotSetupDialog
        property var hotspotManager: null
        objectName: "hotspotSetup"
        anchorToKeyboard: true

        function settingsValid() {
            return ssidField.text != "" && passwordField.length >= 8;
        }


        title: hotspotManager.stored ?
            i18n.tr("Change hotspot setup") : i18n.tr("Setup hotspot")
        text: feedback.enabled ? feedback.text : "";

        Common {
            id: common
        }

        states: [
            State {
                name: "STARTING"
                PropertyChanges {
                    target: workingIndicator
                    running: true
                }
                PropertyChanges {
                    target: ssidLabel
                    opacity: 0.5
                }
                PropertyChanges {
                    target: ssidField
                    enabled: false
                }
                PropertyChanges {
                    target: passwordLabel
                    opacity: 0.5
                }
                PropertyChanges {
                    target: passwordField
                    enabled: false
                }
                PropertyChanges {
                    target: feedback
                    enabled: true
                }
                PropertyChanges {
                    target: enableButton
                    enabled: false
                }
                PropertyChanges {
                    target: changeButton
                    enableD: false
            },
            State {
                name: "FAILED"
                PropertyChanges {
                    target: feedback
                    enabled: true
                }
            },
            State {
                name: "SUCCEEDED"
                PropertyChanges {
                    target: successIndicator
                    running: true
                }
                PropertyChanges {
                    target: ssidLabel
                    opacity: 0.5
                }
                PropertyChanges {
                    target: ssidField
                    enabled: false
                }
                PropertyChanges {
                    target: passwordLabel
                    opacity: 0.5
                }
                PropertyChanges {
                    target: passwordField
                    enabled: false
                }
                PropertyChanges {
                    target: enableButton
                    enabled: false
                }
                PropertyChanges {
                    target: changeButton
                    enabled: false
                }
            }
        ]

        Label {
            property bool enabled: false
            id: feedback
            horizontalAlignment: Text.AlignHCenter
            height: contentHeight
            wrapMode: Text.Wrap
            visible: false
        }

        Label {
            id: ssidLabel
            text: i18n.tr("Hotspot name")
            fontSize: "medium"
            font.bold: true
            color: Theme.palette.selected.backgroundText
            elide: Text.ElideRight
        }

        TextField {
            id: ssidField
            objectName: "ssidField"
            text: hotspotManager.ssid
            Component.onCompleted: forceActiveFocus()
        }

        Label {
            id: passwordLabel
            text: i18n.tr("Key (must be 8 characters or longer)")
            fontSize: "medium"
            font.bold: true
            color: Theme.palette.selected.backgroundText
            elide: Text.ElideRight
        }

        TextField {
            id: passwordField
            objectName: "passwordField"
            text: hotspotManager.password
            echoMode: passwordVisibleSwitch.checked ?
                TextInput.Normal : TextInput.Password
        }

        ListItem.Standard {
            text: i18n.tr("Show key")
            id: passwordVisible
            control: Switch {
                id: passwordVisibleSwitch
            }
        }

        RowLayout {

            anchors.margins: units.gu(2)

            Button {
                id: cancelButton
                Layout.fillWidth: true
                text: i18n.tr("Cancel")
                onClicked: PopupUtils.close(hotspotSetupDialog)
            }

            Button {
                id: enableButton
                objectName: "enableButton"
                visible: !hotspotManager.stored
                Layout.fillWidth: true
                text: i18n.tr("Enable")
                enabled: settingsValid()
                onClicked: enableAction.trigger()
            }

            Button {
                id: changeButton
                visible: hotspotManager.stored
                Layout.fillWidth: true
                text: i18n.tr("Change")
                enabled: enableButton.enabled
                onClicked: changeAction.trigger()
            }
        }

        Item {
            anchors.centerIn: enableButton.visible ? enableButton : changeButton
            width: childrenRect.width
            height: childrenRect.height

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
                id: workingIndicator
                running: false
                visible: running
                height: parent.height - units.gu(1.5)
                anchors {
                    centerIn: parent
                }
            }
        }

        Action {
            id: enableAction
            enabled: settingsValid()
            onTriggered: {
                hotspotManager.ssid = ssidField.text;
                hotspotManager.password = passwordField.text;
                hotspotManager.enabled = true;
                hotspotSetupDialog.state = "STARTING";
            }
        }

        Action {
            id: changeAction
            enabled: settingsValid()
            onTriggered: {

                function hotspotDisabledHandler(enabled) {
                    if (!enabled) {
                        hotspotManager.enabled = true;
                        hotspotManager.enabled.disconnect(hotspotDisabledHandler);
                    }
                }

                hotspotManager.ssid = ssidField.text;
                hotspotManager.password = passwordField.text;

                if (hotspotManager.enabled) {
                    hotspotManager.enabled = false;
                    hotspotSetupDialog.state = "STARTING";
                    hotspotManager.enabled.connect(hotspotDisabledHandler);
                } else {
                    PopupUtils.close(hotspotSetupDialog);
                }
            }
        }

        /* Timer that shows a tick in the connect button once we have
        successfully changed/started a hotspot. */
        Timer {
            id: successIndicator
            interval: 2000
            running: false
            repeat: false
            onTriggered: PopupUtils.close(hotspotSetupDialog)
        }

        Connections {
            target: hotspotManager

            onReportError: {
                if (hotspotSetupDialog.state === "CONNECTING") {
                    feedback.text = common.reasonToString(reason);
                    otherNetworkDialog.state = "FAILED";
                }
            }
        }
    }
}
