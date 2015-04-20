/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Pat McGowan <pat.mcgowan@canonical.com>
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
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 1.0 as ListItem

Dialog {
    id: root

    // Property that holds the APN (apn.js) module.
    property var apnLib

    // MMS, Internet or LTE model
    property var contextModel

    property bool isMms: contextModel.type === "mms"
    property bool isIinternet: contextModel.type === "internet"
    property bool isLte: contextModel.type === "lte"

    // When user activates
    signal activated (string contextPath)

    // When user cancels
    signal canceled ()

    states: [
        State {
            name: "activating"
            PropertyChanges {
                target: connectButtonIndicator
                running: true
            }
            PropertyChanges {
                target: confirmButton
                text: ""
                enabled: false
            }
        },
        State {
            name: "activatingDone"
            PropertyChanges {
                target: activatingDoneTimer
                running: true
            }
            PropertyChanges {
                target: confirmButton
                text: ""
                enabled: false
            }
            PropertyChanges {
                target: cancelButton
                enabled: false
            }
        },
        State {
            name: "activateFailed"
            PropertyChanges {
                target: root
                text: i18n.tr("APN activation failed.")
            }
        }
    ]

    Component.onCompleted: {
        console.warn('opened editor for type', contextModel.type)
        // if (apn) {
        //     d.populateInputs(apn);
        // } else if (activeInternetApn) {
        //     d.populateInputs(activeInternetApn);
        // }
    }

    // Main column, holding all controls and buttons.
    Column {
        anchors { left: parent.left; right: parent.right }
        spacing: units.gu(2)

        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)
            visible: contextModel.count

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("Suggestions")
            }

            ListItem.ItemSelector {
                id: suggestions
                model: contextModel
                visible: contextModel.count
                expanded: true
                delegate: OptionSelectorDelegate {
                    property string name: contextModel.get(index).qml.name
                    text: apnLib.isNameCustom(name) ? i18n.tr("Custom") : name
                    showDivider: false
                }
                //onDelegateClicked: APN.activateContext(model.get(index))
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
                width: parent.width
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                Component.onCompleted: forceActiveFocus()
            }
        }

        // MMSC controls
        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)
            visible: isMms

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("MMSC:")
            }

            TextField {
                id: messageCenter
                function setHttp(link) {
                    if (link.search(/^http[s]?\:\/\//) == -1) {
                        link = 'http://' + link;
                    }
                    return link;
                }
                width: parent.width
                inputMethodHints: Qt.ImhUrlCharactersOnly |
                                  Qt.ImhNoAutoUppercase |
                                  Qt.ImhNoPredictiveText
                onFocusChanged: {
                    if (!focus && text.length > 0) {
                        text = setHttp(text);
                    }
                }
            }
        }

        // Proxy controls
        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)
            visible: isMms

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("Proxy:")
            }

            TextField {
                id: messageProxy
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
            visible: isMms

            Label {
                wrapMode: Text.WrapAnywhere
                id: portLabel
                text: i18n.tr("Proxy port:")
                width: contentWidth
                height: parent.height
                verticalAlignment: Text.AlignVCenter
            }

            TextField {
                id: port
                width: (parent.width - portLabel.width) - parent.spacing
                maximumLength: 5
                inputMethodHints: Qt.ImhDigitsOnly |
                                  Qt.ImhNoAutoUppercase |
                                  Qt.ImhNoPredictiveText
                validator: portValidator
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
                width: parent.width
                enabled: !doBoth.checked
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhSensitiveData
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
                width: parent.width
                echoMode: passwordHiddenSwitch.checked ?
                    TextInput.Password : TextInput.Normal
                inputMethodHints: Qt.ImhNoAutoUppercase |
                                  Qt.ImhNoPredictiveText |
                                  Qt.ImhSensitiveData
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
                id: confirmButton
                width: (parent.width / 2) - units.gu(1)
                text: i18n.tr("Activate")
                enabled: true // TODO: isValid && isDifferent
                activeFocusOnPress: false

                onClicked: {}

                Icon {
                    height: parent.height - units.gu(1.5)
                    width: parent.height - units.gu(1.5)
                    anchors {
                        centerIn: parent
                    }
                    name: "tick"
                    color: "green"
                    visible: activatingDoneTimer.running
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
        } // row for buttons

    } // main column holding all controls and buttons

    Timer {
        id: activatingDoneTimer
        interval: 2000
        onTriggered: root.canceled()
    }
}
