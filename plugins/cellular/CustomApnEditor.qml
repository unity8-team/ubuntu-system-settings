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

    property var apnLib

    // MMS, Internet or LTE model
    property var contextModel

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

    QtObject {
        id: d
        property bool isMms : root.type === "mms"

        // Whether or not input data is valid
        property bool isValid : {
            if (apnName.text === "") {
                return false;
            }
            if (isMms) {
                if (mmsc.text === "") {
                    return false;
                }
                if (proxy.text === "") {
                    return false;
                }
                if (!port.acceptableInput) {
                    return false;
                }
            }

            // @todo the rest
            return true;
        }

        // Whether or input from user is different to what we started with
        property bool isChanged : {
            if (apn) {
                if (apnName.text !== apn.accessPointName) { return true; }
                if (mmsc.text !== apn.messageCenter) { return true; }
                if (proxy.text !== apn.settings["proxy"]) { return true; }
                if (port.text !== apn.settings["port"]) { return true; }
                if (username.text !== apn.username) { return true; }
                if (password.text !== apn.username) { return true; }
                return false;
            } else {
                return true;
            }
        }

        /*
        @param {Object} apn to use when populating inputs
        @return undefined
        */
        function populateInputs (apn) {
            var mProxy = apn.messageProxy;
            if (apn.accessPointName) apnName.text = apn.accessPointName;
            if (apn.messageCenter) mmsc.text = apn.messageCenter;
            if (mProxy) {

                proxy.text = mProxy.slice(0, mProxy.indexOf(':') >= 0 ?
                    mProxy.indexOf(':') : mProxy.length);
                port.text = mProxy.slice(mProxy.indexOf(':') + 1);
            }
            if (apn.username) username.text = apn.username;
            if (apn.password) password.text = apn.password;
        }

        /*
        @return {Object} dict of data that was entered
        */
        function getEnteredData () {
            return {
                type: root.type,
                name :apnName.text,
                mmsc: mmsc.text,
                proxy: proxy.text,
                port: port.text,
                username: username.text,
                password: password.text
            }
        }
    }

    Column {
        anchors { left: parent.left; right: parent.right }
        spacing: units.gu(2)

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
            }

            Button {
                id: confirmButton
                width: (parent.width / 2) - units.gu(1)
                text: i18n.tr("Activate")
                enabled: d.isValid && d.isChanged

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

        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)
            visible: true

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
                text: i18n.tr("APN")
            }

            TextField {
                id: apnName
                width: parent.width
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                Component.onCompleted: forceActiveFocus()
            }
        }

        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)
            visible: d.isMms

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("MMSC")
            }

            TextField {

                id: mmsc

                function setHttp(link) {
                    if (link.search(/^http[s]?\:\/\//) == -1) {
                        link = 'http://' + link;
                    }
                    return link;
                }

                width: parent.width
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                onHighlightedChanged: {
                    if (!highlighted) {
                        text = setHttp(text);
                    }
                }
            }
        }

        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)
            visible: d.isMms

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("Proxy")
            }

            TextField {
                id: proxy
                width: parent.width
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                onTextChanged: {
                    movePortDelay.running = false;
                    if (text.indexOf(':') >= 0) {
                        movePortDelay.running = true;
                    }
                }

            }

            Timer {
                id: movePortDelay
                interval: 1000
                onTriggered: {
                    var portIndex = proxy.text.indexOf(':');
                    var textSansPort = proxy.text.slice(0, portIndex)
                    var prt = proxy.text.slice(portIndex + 1);

                    if (prt) {
                        proxy.text = textSansPort;
                        port.text = prt;
                        port.forceActiveFocus();
                    }
                }
            }
        }

        Row {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(2)
            visible: d.isMms

            Label {
                wrapMode: Text.WrapAnywhere
                id: portLabel
                text: i18n.tr("Port")
                width: contentWidth
                height: parent.height
                verticalAlignment: Text.AlignVCenter
            }

            TextField {
                id: port
                width: (parent.width - portLabel.width) - parent.spacing
                maximumLength: 4
                inputMethodHints: Qt.ImhDigitsOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                validator: portValidator
            }

            RegExpValidator {
                id: portValidator
                regExp: /\d{1,4}/
            }
        }

        Column {
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(0.5)
            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("Username")
            }
            TextField {
                id: username
                width: parent.width
                enabled: !doBoth.checked
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            }
        }

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
                enabled: !doBoth.checked
                echoMode: passwordVisibleSwitch.checked ?
                    TextInput.Normal : TextInput.Password
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            }

            Row {
                id: passwordVisiblityRow
                layoutDirection: Qt.LeftToRight
                spacing: units.gu(1)

                CheckBox {
                    id: passwordVisibleSwitch
                    activeFocusOnPress: false
                }

                Label {
                    id: passwordVisibleLabel
                    text : i18n.tr("Show password")
                    objectName: "passwordVisibleLabel"
                    fontSize: "medium"
                    color: Theme.palette.selected.backgroundText
                    elide: Text.ElideRight
                    height: passwordVisibleSwitch.height
                    verticalAlignment: Text.AlignVCenter
                    MouseArea {
                        anchors {
                            fill: parent
                        }
                        onClicked: {
                            passwordVisibleSwitch.checked =
                                !passwordVisibleSwitch.checked
                        }
                    }
                }
            }



        }
        /// @todo support for ipv6 will be added after RTM

    } // the contents

    Timer {
        id: activatingDoneTimer
        interval: 2000
        onTriggered: root.canceled()
    }
}
