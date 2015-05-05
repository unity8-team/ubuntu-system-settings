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
import "Components" as LocalComponents
import "apn_editor.js" as Editor

ItemPage {
    id: root
    objectName: "apnEditor"

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

    // When a user has requested saving a context.
    signal saving ()

    // When user has saved a context.
    signal saved ()

    // Signal for new contexts.
    signal newContext (OfonoContextConnection context)

    // When user cancels.
    signal canceled ()

    title: contextQML ? i18n.tr("Edit") : i18n.tr("New APN")

    state: "default"
    states: [
        PageHeadState {
            name: "default"
            head: root.head
            actions: [
                Action {
                    iconName: "ok"
                    enabled: isValid
                    onTriggered: Editor.saving()
                }
            ]
        },
        PageHeadState {
            name: "busy"
            head: root.head
            actions: [
                Action {
                    iconName: "ok"
                    enabled: false
                }
            ]
        },
        State {
            name: "busy"
            PropertyChanges { target: name; enabled: false; }
            PropertyChanges { target: accessPointName; enabled: false; }
            PropertyChanges { target: username; enabled: false; }
            PropertyChanges { target: password; enabled: false; }
            PropertyChanges { target: messageCenter; enabled: false; }
            PropertyChanges { target: messageProxy; enabled: false; }
            PropertyChanges { target: port; enabled: false; }
        }
    ]

    onSaving: state = "busy"
    onSaved: pageStack.pop();
    onCanceled: pageStack.pop();
    onNewContext: Editor.newContext(context);

    Component.onCompleted: {
        if (contextQML) {
            Editor.populate(contextQML);
        }
    }

    // Main column, holding all controls and buttons.
    Flickable {
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
            Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors { left: parent.left; right: parent.right; }

            // Type controls
            Column {
                anchors { left: parent.left; right: parent.right; }

                SettingsItemTitle {
                    text: i18n.tr("Used for:")
                }

                ListItem.ItemSelector {
                    model: [i18n.tr('Internet and MMS'),
                            i18n.tr('Internet'),
                            i18n.tr('MMS'),
                            i18n.tr('LTE'), ]
                    id: typeSel
                    objectName: "type"
                    width: parent.width
                    KeyNavigation.tab: name
                }
            }

            // Name controls
            Column {
                anchors { left: parent.left; right: parent.right; }

                SettingsItemTitle {
                    anchors { left: parent.left; right: parent.right }
                    text: i18n.tr("Name")
                }

                LocalComponents.LabelTextField {
                    id: name
                    objectName: "name"
                    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                    placeholderText: i18n.tr("Enter a name that describes this
                    context")
                    KeyNavigation.tab: accessPointName
                }
            }

            // APN controls
            Column {
                anchors { left: parent.left; right: parent.right; }

                SettingsItemTitle {
                    anchors { left: parent.left; right: parent.right }
                    text: i18n.tr("APN")
                }

                LocalComponents.LabelTextField {
                    id: accessPointName
                    objectName: "accessPointName"
                    inputMethodHints: Qt.ImhUrlCharactersOnly |
                                      Qt.ImhNoAutoUppercase |
                                      Qt.ImhNoPredictiveText
                    placeholderText: i18n.tr("Enter the name of the access point")
                    KeyNavigation.tab: isMms || isCombo ? messageCenter : username
                }
            }

            // MMSC controls
            Column {
                anchors { left: parent.left; right: parent.right }
                visible: isMms || isCombo

                SettingsItemTitle {
                    anchors { left: parent.left; right: parent.right }
                    text: i18n.tr("MMSC")
                }

                LocalComponents.LabelTextField {
                    id: messageCenter
                    objectName: "messageCenter"
                    inputMethodHints: Qt.ImhUrlCharactersOnly |
                                      Qt.ImhNoAutoUppercase |
                                      Qt.ImhNoPredictiveText
                    onFocusChanged: {
                        if (!focus && text.length > 0) {
                            text = Editor.setHttp(text);
                        }
                    }
                    placeholderText: i18n.tr("Enter message center")
                    KeyNavigation.tab: messageProxy
                }
            }

            // Proxy controls
            Column {
                anchors { left: parent.left; right: parent.right }
                visible: isMms || isCombo

                SettingsItemTitle {
                    anchors { left: parent.left; right: parent.right }
                    text: i18n.tr("Proxy")
                }

                LocalComponents.LabelTextField {
                    id: messageProxy
                    objectName: "messageProxy"
                    inputMethodHints: Qt.ImhUrlCharactersOnly |
                                      Qt.ImhNoAutoUppercase |
                                      Qt.ImhNoPredictiveText
                    onTextChanged: {
                        movePortDelay.running = false;
                        if (text.search(/\:\d+$/) >= 0) {
                            movePortDelay.running = true;
                        }
                    }
                    placeholderText: i18n.tr("Enter message proxy")
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
            Column {
                anchors { left: parent.left; right: parent.right }
                visible: isMms || isCombo

                SettingsItemTitle {
                    id: portLabel
                    text: i18n.tr("Proxy port")
                }

                LocalComponents.LabelTextField {
                    id: port
                    objectName: "port"
                    maximumLength: 5
                    inputMethodHints: Qt.ImhDigitsOnly |
                                      Qt.ImhNoAutoUppercase |
                                      Qt.ImhNoPredictiveText
                    validator: portValidator
                    placeholderText: i18n.tr("Enter message proxy port")
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

                SettingsItemTitle {
                    width: parent.width
                    text: i18n.tr("User name")
                }

                LocalComponents.LabelTextField {
                    id: username
                    objectName: "username"
                    inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhSensitiveData
                    placeholderText: i18n.tr("Enter username")
                    KeyNavigation.tab: password
                }
            }

            // Password controls
            Column {
                anchors { left: parent.left; right: parent.right }

                SettingsItemTitle {
                    width: parent.width
                    text: i18n.tr("Password")
                }

                LocalComponents.LabelTextField {
                    id: password
                    objectName: "password"
                    width: parent.width
                    echoMode: TextInput.Normal
                    inputMethodHints: Qt.ImhNoAutoUppercase |
                                      Qt.ImhNoPredictiveText |
                                      Qt.ImhSensitiveData
                    placeholderText: i18n.tr("Enter password")
                }
            }

            // Authentication controls
            Column {
                anchors { left: parent.left; right: parent.right }
                visible: showAllUI

                SettingsItemTitle {
                    width: parent.width
                    text: i18n.tr("Authentication")
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
                visible: showAllUI

                SettingsItemTitle {
                    width: parent.width
                    text: i18n.tr("Protocol")
                }

                ListItem.ItemSelector {
                    model: [i18n.tr("IPv4"),
                            i18n.tr("IPv6"),
                            i18n.tr("IPv4v6")]
                }
            }
        }
    } // main column holding all controls and buttons

    Timer {
        id: updateContext
        property OfonoContextConnection ctx
        interval: 1500
        onTriggered: {
            Editor.updateContextQML(ctx);
            root.saved();
        }
    }
}
