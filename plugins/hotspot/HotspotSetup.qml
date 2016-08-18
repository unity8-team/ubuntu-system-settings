/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014-2016 Canonical Ltd.
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

import QtQuick 2.4
import QtQuick.Layouts 1.1
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Components.Popups 1.3
import Ubuntu.Connectivity 1.0
import Ubuntu.SystemSettings.Cellular 1.0

Component {
    id: hotspotSetup

    Dialog {
        id: hotspotSetupDialog

        /* Connectivity.hotspotStored changes as soon as the user has added a
        hotspot, and we use this value when we choose between e.g. "Change" and
        "Setup". We'd like the narrative to be consistent, so we stick with
        what the stored value was at component completion.
        */
        property bool stored: false
        Component.onCompleted: stored = Connectivity.hotspotStored;

        objectName: "hotspotSetup"
        anchorToKeyboard: true

        function settingsValid() {
            var ssidValid = ssidField.text !== "";
            var passwordValid = passwordRequiredToggle.checked ?
                                passwordField.length >= 8 : true;
            return ssidValid && passwordValid;
        }

        function updateHotspotSettings () {
            Connectivity.hotspotSsid = ssidField.text;
            Connectivity.hotspotPassword = passwordField.text;
            Connectivity.hotspotAuth = passwordRequiredToggle.checked ?
                                       "wpa-psk" : "none";
        }

        title: stored ?
            i18n.tr("Change Hotspot Setup") : i18n.tr("Set Up Hotspot")
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
                    target: passwordRequired
                    enabled: false
                }
                PropertyChanges {
                    target: passwordRequiredLabel
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
                    target: confirmButton
                    enabled: false
                }
            },

            State {
                name: "FAILED"
                PropertyChanges {
                    target: feedback
                    enabled: true
                }
                PropertyChanges {
                    target: ssidField
                    errorHighlight: true
                }
                StateChangeScript {
                    script: ssidField.forceActiveFocus()
                }
            },

            State {
                name: "SUCCEEDED"
                PropertyChanges {
                    target: successIcon
                    visible: true
                }
                PropertyChanges {
                    target: successIndicator
                    running: true
                }
                PropertyChanges {
                    target: ssidLabel
                    opacity: 0.5
                }
                PropertyChanges {
                    target: passwordRequired
                    enabled: false
                }
                PropertyChanges {
                    target: passwordRequiredLabel
                    opacity: 0.5
                }
                PropertyChanges {
                    target: ssidField
                    enabled: false
                }
                PropertyChanges {
                    target: passwordField
                    enabled: false
                }
                PropertyChanges {
                    target: confirmButton
                    enabled: false
                }
            }
        ]

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(1)

            Label {
                property bool enabled: false
                id: feedback
                horizontalAlignment: Text.AlignHCenter
                height: contentHeight
                wrapMode: Text.WordWrap
                visible: false
            }

            Label {
                id: ssidLabel
                text: hotspotSetupDialog.stored ? i18n.tr("Hotspot name") :
                                                  i18n.tr("Choose a name")
                font.bold: true
                color: Theme.palette.normal.baseText
                elide: Text.ElideRight
            }

            TextField {
                id: ssidField
                objectName: "ssidField"
                text: Connectivity.hotspotSsid
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                Component.onCompleted: forceActiveFocus()
                width: parent.width
            }

            SettingsListItems.Standard {
                id: passwordRequired
                text: i18n.tr("Require a password (recommended):")
                showDivider: false
                layout.padding.leading: 0
                SlotsLayout.padding.leading: 0

                CheckBox {
                    id: passwordRequiredToggle
                    objectName: "passwordRequiredToggle"
                    checked: Connectivity.hotspotAuth === "wpa-psk"
                    SlotsLayout.position: SlotsLayout.First
                    SlotsLayout.padding.leading: 0
                    // FIXME: Workaround for lp:1415023
                    activeFocusOnPress: false
                }
                onClicked: passwordRequiredToggle.trigger()
            }

            TextField {
                id: passwordField
                objectName: "passwordField"
                enabled: passwordRequiredToggle.checked
                text: Connectivity.hotspotPassword
                echoMode: passwordVisibleToggle.checked ?
                    TextInput.Normal : TextInput.Password
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                width: parent.width
            }

            SettingsListItems.Standard {
                id: passwordVisible
                enabled: passwordRequiredToggle.checked
                text: i18n.tr("Show password")
                layout.padding.leading: 0
                SlotsLayout.padding.leading: 0

                CheckBox {
                    id: passwordVisibleToggle
                    enabled: parent.enabled
                    SlotsLayout.position: SlotsLayout.First
                    SlotsLayout.padding.leading: 0
                    // FIXME: Workaround for lp:1415023
                    activeFocusOnPress: false
                }
                onClicked: passwordVisibleToggle.trigger()
            }

            ListItems.Caption {
                id: enableWifiCaption
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: i18n.tr("Starting the hotspot will turn on Wi-Fi.")
                visible: !Connectivity.wifiEnabled &&
                         !hotspotSetupDialog.stored &&
                         hotspotSetupDialog.state !== "SUCCEEDED"
            }

            Row {

                anchors {
                    left: parent.left
                    right: parent.right
                }
                width: parent.width
                spacing: units.gu(2)

                Button {
                    id: cancelButton
                    width: (parent.width / 2) - units.gu(1)
                    text: i18n.tr("Cancel")

                    // FIXME: Workaround for lp:1415023
                    activeFocusOnPress: false
                    onClicked: PopupUtils.close(hotspotSetupDialog)
                }

                Button {
                    id: confirmButton
                    objectName: "confirmButton"
                    width: (parent.width / 2) - units.gu(1)
                    text: hotspotSetupDialog.stored ? i18n.tr("Change") :
                        i18n.tr("Start")
                    enabled: settingsValid()

                    // FIXME: Workaround for lp:1415023
                    activeFocusOnPress: false
                    onClicked: {
                        if (!Connectivity.wifiEnabled &&
                                !hotspotSetupDialog.stored) {
                            enableWifiAction.trigger();
                        } else if (hotspotSetupDialog.stored) {
                            changeAction.trigger()
                        } else {
                            enableAction.trigger();
                        }
                    }

                    Icon {
                        id: successIcon
                        anchors.centerIn: parent
                        height: parent.height - units.gu(1.5)
                        width: parent.height - units.gu(1.5)
                        name: "tick"
                        color: "green"
                        visible: false
                    }

                    ActivityIndicator {
                        id: workingIndicator
                        anchors.centerIn: parent
                        running: false
                        visible: running
                        height: parent.height - units.gu(1.5)
                    }
                }
            }
        }

        Action {
            id: enableWifiAction
            onTriggered: {
                hotspotSetupDialog.state = "STARTING";

                // As soon as Wi-Fi has been turned on, trigger enableAction.
                function wifiUpdated (updated) {
                    Connectivity.wifiEnabledUpdated.disconnect(wifiUpdated);
                    enableAction.trigger();
                }

                Connectivity.wifiEnabledUpdated.connect(wifiUpdated);
                Connectivity.wifiEnabled = true;
            }
        }

        Action {
            id: enableAction
            enabled: settingsValid()
            onTriggered: {
                hotspotSetupDialog.state = "STARTING";

                function hotspotEnabledHandler (enabled) {
                    if (enabled) {
                        hotspotSetupDialog.state = "SUCCEEDED";
                        Connectivity.hotspotEnabledUpdated.disconnect(
                            hotspotEnabledHandler);
                    }
                }

                hotspotSetupDialog.updateHotspotSettings();
                Connectivity.hotspotEnabledUpdated.connect(hotspotEnabledHandler);
                Connectivity.hotspotEnabled = true;
            }
        }

        Action {
            id: changeAction
            enabled: settingsValid()
            onTriggered: {

                function hotspotEnabledHandler (enabled) {
                    if (enabled) {
                        hotspotSetupDialog.state = "SUCCEEDED";
                        Connectivity.hotspotEnabledUpdated.disconnect(
                            hotspotEnabledHandler);
                    }
                }

                function hotspotDisabledHandler (enabled) {
                    if (!enabled) {
                        Connectivity.hotspotEnabledUpdated.connect(hotspotEnabledHandler);
                        Connectivity.hotspotEnabled = true;
                        Connectivity.hotspotEnabledUpdated.disconnect(
                            hotspotDisabledHandler);
                    }
                }

                hotspotSetupDialog.updateHotspotSettings();

                if (Connectivity.hotspotEnabled) {
                    hotspotSetupDialog.state = "STARTING";
                    Connectivity.hotspotEnabledUpdated.connect(
                        hotspotDisabledHandler);
                    Connectivity.hotspotEnabled = false;
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
            target: Connectivity

            onReportError: {
                if (hotspotSetupDialog.state === "STARTING") {
                    hotspotSetupDialog.state = "FAILED";
                    feedback.text = common.reasonToString(reason);
                }
            }
        }
    }
}
