/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Pat McGowan <pat.mcgowan@canonical.com>,
 *          Jonas G. Drange <jonas.drange@canonical.com>
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
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem
import MeeGo.QOfono 0.2
import "apn_editor.js" as Editor

Dialog {
    id: root
    objectName: "customApnEditor"

    // Property that holds the APN manager (apn_manager.js) module.
    property var manager

    property OfonoContextConnection contextQML

    // All models.
    property ListModel mmsModel
    property ListModel internetModel
    property ListModel iaModel

    // Flags that indicate what context we are editing.
    property bool isCombo: Editor.indexToType(typeSel.selectedIndex) === 'internet+mms'
    property bool isInternet: Editor.indexToType(typeSel.selectedIndex) === 'internet'
    property bool isMms: Editor.indexToType(typeSel.selectedIndex) === 'mms'
    property bool isIa: Editor.indexToType(typeSel.selectedIndex) === 'ia'

    property bool isValid: Editor.isValid()

    // When user activates.
    signal saved (OfonoContextConnection context)

    // When user cancels.
    signal canceled ()

    Component.onCompleted: {
        if (contextQML) {
            Editor.populate(contextQML);
        }
    }

    // Main column, holding all controls and buttons.
    Column {
        anchors { left: parent.left; right: parent.right }
        spacing: units.gu(0.5)

        // Type controls
        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("Used for:")
            }

            ListItem.ItemSelector {
                model: [i18n.tr('Internet and MMS'),
                        i18n.tr('Internet'),
                        i18n.tr('MMS'),
                        i18n.tr('IA'), ]
                id: typeSel
                objectName: "type"
                width: parent.width
                KeyNavigation.tab: name
            }
        }

        // Name controls
        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("Name:")
            }

            TextField {
                id: name
                objectName: "name"
                width: parent.width
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                Component.onCompleted: forceActiveFocus()
                KeyNavigation.tab: accessPointName
            }
        }

        // APN controls
        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("APN:")
            }

            TextField {
                id: accessPointName
                objectName: "accessPointName"
                width: parent.width
                inputMethodHints: Qt.ImhUrlCharactersOnly |
                                  Qt.ImhNoAutoUppercase |
                                  Qt.ImhNoPredictiveText
                KeyNavigation.tab: isMms || isCombo ? messageCenter : username
            }
        }

        // MMSC controls
        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)
            visible: isMms || isCombo

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("MMSC:")
            }

            TextField {
                id: messageCenter
                objectName: "messageCenter"
                width: parent.width
                inputMethodHints: Qt.ImhUrlCharactersOnly |
                                  Qt.ImhNoAutoUppercase |
                                  Qt.ImhNoPredictiveText
                onFocusChanged: {
                    if (!focus && text.length > 0) {
                        text = Editor.setHttp(text);
                    }
                }
                KeyNavigation.tab: messageProxy
            }
        }

        // Proxy controls
        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)
            visible: isMms || isCombo

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("Proxy:")
            }

            TextField {
                id: messageProxy
                objectName: "messageProxy"
                width: parent.width
                inputMethodHints: Qt.ImhUrlCharactersOnly |
                                  Qt.ImhNoAutoUppercase |
                                  Qt.ImhNoPredictiveText
                onTextChanged: {
                    movePortDelay.running = false;
                    if (text.search(/\:\d+$/) >= 0) {
                        movePortDelay.running = true;
                    }
                }
                KeyNavigation.tab: port
            }

            Timer {
                id: movePortDelay
                interval: 1000
                onTriggered: {

                    function getPort(s) {
                        var match = s.match(/\:(\d+)/);
                        if (match === null) {
                            return null;
                        } else {
                            return match[1];
                        }
                    }

                    var prt = getPort(messageProxy.text);
                    var portIndex = messageProxy.text.indexOf(prt);
                    var textSansPort = messageProxy.text.slice(0, portIndex - 1);

                    if (prt) {
                        messageProxy.text = textSansPort;
                        port.text = prt;
                        port.forceActiveFocus();
                    }
                }
            }
        }

        // Proxy port controls
        Row {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(2)
            visible: isMms || isCombo

            Label {
                id: portLabel
                wrapMode: Text.WrapAnywhere
                text: i18n.tr("Proxy port:")
                width: contentWidth
                height: parent.height
                verticalAlignment: Text.AlignVCenter
            }

            TextField {
                id: port
                objectName: "port"
                width: (parent.width - portLabel.width) - parent.spacing
                maximumLength: 5
                inputMethodHints: Qt.ImhDigitsOnly |
                                  Qt.ImhNoAutoUppercase |
                                  Qt.ImhNoPredictiveText
                validator: portValidator
                KeyNavigation.tab: username
            }

            RegExpValidator {
                id: portValidator
                regExp: /([0-9]{1,4}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])/
            }
        }

        // Username controls
        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("User name:")
            }

            TextField {
                id: username
                objectName: "username"
                width: parent.width
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhSensitiveData
                KeyNavigation.tab: password
            }
        }

        // Password controls
        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("Password")
            }

            TextField {
                id: password
                objectName: "password"
                width: parent.width
                echoMode: passwordHiddenSwitch.checked ?
                    TextInput.Password : TextInput.Normal
                inputMethodHints: Qt.ImhNoAutoUppercase |
                                  Qt.ImhNoPredictiveText |
                                  Qt.ImhSensitiveData
                KeyNavigation.tab: activateButton
            }

            // FIXME: Use ListItem control?
            Row {
                id: passwordVisiblityRow
                layoutDirection: Qt.LeftToRight
                spacing: units.gu(1)

                CheckBox {
                    id: passwordHiddenSwitch
                    activeFocusOnPress: false
                }

                Label {
                    id: passwordVisibleLabel
                    text : i18n.tr("Hide password")
                    objectName: "passwordVisibleLabel"
                    fontSize: "medium"
                    color: Theme.palette.selected.backgroundText
                    elide: Text.ElideRight
                    height: passwordHiddenSwitch.height
                    verticalAlignment: Text.AlignVCenter
                    MouseArea {
                        anchors {
                            fill: parent
                        }
                        onClicked: {
                            passwordHiddenSwitch.checked =
                                !passwordHiddenSwitch.checked
                        }
                    }
                }
            }
        }

        // Authentication controls
        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)
            visible: showAllUI

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("Authentication:")
            }

            ListItem.ItemSelector {
                model: [i18n.tr("None"),
                        i18n.tr("PAP or CHAP"),
                        i18n.tr("PAP only"),
                        i18n.tr("CHAP only")]
            }
        }

        // Protocol controls
        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)
            visible: showAllUI

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("Protocol:")
            }

            ListItem.ItemSelector {
                model: [i18n.tr("IPv4"),
                        i18n.tr("IPv6"),
                        i18n.tr("IPv4v6")]
            }
        }

        // Button row.
        Row {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(2)
            height: cancelButton.height

            Button {
                id: cancelButton
                width: (parent.width / 2) - units.gu(1)
                text: i18n.tr("Cancel")
                onClicked: root.canceled()
                activeFocusOnPress: false
            }

            Button {
                id: activateButton
                objectName: "activateButton"
                width: (parent.width / 2) - units.gu(1)
                text: i18n.tr("Save")
                enabled: isValid
                activeFocusOnPress: false

                onClicked: Editor.saving()
            }
        } // row for buttons

    } // main column holding all controls and buttons
}
