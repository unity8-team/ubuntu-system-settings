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
import Ubuntu.Connectivity 1.0
import Ubuntu.SystemSettings.Cellular 1.0
import Ubuntu.Components.Popups 0.1

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
            return ssidField.text != "" && passwordField.length >= 8;
        }

        title: stored ?
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
                text: i18n.tr("Hotspot name")
                fontSize: "medium"
                font.bold: true
                color: Theme.palette.selected.backgroundText
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

            Label {
                id: passwordLabel
                text: i18n.tr("Key (must be 8 characters or longer)")
                fontSize: "medium"
                font.bold: true
                color: Theme.palette.selected.backgroundText
                wrapMode: Text.WordWrap
                width: parent.width
            }

            TextField {
                id: passwordField
                objectName: "passwordField"
                text: Connectivity.hotspotPassword
                echoMode: passwordVisibleSwitch.checked ?
                    TextInput.Normal : TextInput.Password
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                width: parent.width
            }

            ListItem.Standard {
                text: i18n.tr("Show key")
                id: passwordVisible
                onClicked: passwordVisibleSwitch.trigger()
                control: Switch {
                    id: passwordVisibleSwitch
                    activeFocusOnPress: false
                }
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
                    activeFocusOnPress: false
                    onClicked: PopupUtils.close(hotspotSetupDialog)
                }

                Button {
                    id: confirmButton
                    objectName: "confirmButton"
                    width: (parent.width / 2) - units.gu(1)
                    text: hotspotSetupDialog.stored ? i18n.tr("Change") :
                        i18n.tr("Enable")
                    enabled: settingsValid()
                    activeFocusOnPress: false
                    onClicked: {
                        if (hotspotSetupDialog.stored) {
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

                Connectivity.hotspotSsid = ssidField.text;
                Connectivity.hotspotPassword = passwordField.text;
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

                Connectivity.hotspotSsid = ssidField.text;
                Connectivity.hotspotPassword = passwordField.text;

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
