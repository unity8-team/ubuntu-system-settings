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
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 1.0 as ListItem

Dialog {
    id: root

    // Property that holds the APN (apn.js) module.
    property var apnLib

    // MMS, Internet or ia model
    property var contextModel

    // All models
    property var mmsModel
    property var internetModel
    property var iaModel

    property bool isMms: contextModel.type === "mms"
    property bool isInternet: contextModel.type === "internet"
    property bool isIa: contextModel.type === "ia"

    property var suggestion: null

    property bool isValid: {
        return accessPointName.text;
    }
    property bool isChanged: {

        // We had no suggestion, so we have nothing to compare it to.
        if (!suggestion) {
            return true;
        }

        var refData = [suggestion.accessPointName,
                       suggestion.username,
                       suggestion.password];
        var formData = [accessPointName.text,
                        username.text,
                        password.text];
        var i;

        // If we are comparing mms, add some more data to ref and form arrays.
        if (isMms) {
            refData.push(suggestion.messageCenter);
            formData.push(messageCenter.text);

            refData.push(suggestion.messageProxy);
            formData.push(messageProxy.text + (port.text ? ':' + port.text : ''));
        }

        // Compare the arrays.
        i = refData.length;
        while(i--) {
            if (refData[i] !== formData[i]) return true;
        }
        return false;
    }


    // When user activates
    signal activated ()

    // When activation failed
    signal failed ()

    // When activation suceeds
    signal succeeded ()

    // When user cancels
    signal canceled ()

    states: [
        State {
            name: "deactivateEverything"
            PropertyChanges { target: copyFromInternet; enabled: false; }
            PropertyChanges { target: copyFromMms; enabled: false; }
            PropertyChanges { target: suggestions; enabled: false; }
            PropertyChanges { target: accessPointName; enabled: false; }
            PropertyChanges { target: username; enabled: false; }
            PropertyChanges { target: password; enabled: false; }
            PropertyChanges { target: messageCenter; enabled: false; }
            PropertyChanges { target: messageProxy; enabled: false; }
            PropertyChanges { target: port; enabled: false; }
            PropertyChanges { target: passwordHiddenSwitch; enabled: false; }
            PropertyChanges { target: confirmButton; enabled: false; }
            PropertyChanges { target: cancelButton; enabled: false; }
        },

        State {
            name: "activating"
            extend: "deactivateEverything"
            PropertyChanges {
                target: connectButtonIndicator
                running: true
            }
            PropertyChanges {
                target: confirmButton
                text: ""
                enabled: false
            }
            PropertyChanges {
                target: cancelButton
                enabled: true
            }
        },

        State {
            name: "activatingDone"
            extend: "deactivateEverything"
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
            StateChangeScript {
                name: "scroll"
                script: accessPointName.forceActiveFocus();
            }
        }
    ]

    onActivated: state = "activating"
    onFailed: state = "activateFailed"
    onSucceeded: state = "activatingDone"

    // Main column, holding all controls and buttons.
    Column {
        anchors { left: parent.left; right: parent.right }
        spacing: units.gu(2)

        // Suggestion controls
        Column {

            function fill (context) {
                accessPointName.text = context.accessPointName;
                username.text = context.username;
                password.text = context.password;

                if (isMms) {
                    messageCenter.text = context.messageCenter;
                    messageProxy.text = context.messageProxy;
                }
            }

            visible: contextModel.count ||
                     (isIa && internetModel.count) ||
                     (isInternet && mmsModel.count)
            anchors { left: parent.left; right: parent.right }

            Label {
                wrapMode: Text.WrapAnywhere
                width: parent.width
                text: i18n.tr("Suggestions")
            }

            Item { width: parent.width; height: units.gu(1); }

            // The default suggestion for ia contexts, allowing the user
            // to copy from Internet context.
            ListItem.ItemSelector {
                id: copyFromInternet
                visible: isIa && internetModel.count
                expanded: true
                model: 1
                selectedIndex: -1
                delegate: OptionSelectorDelegate {
                    text: i18n.tr("Copy from Internet APN")
                    showDivider: false
                }

                onDelegateClicked: {
                    suggestions.selectedIndex = -1;
                    parent.fill(internetModel.get(0).qml);
                }

                // Dirty hack to remove unavoidable line under an ItemSelector.
                Rectangle {
                    anchors.bottom: parent.bottom; anchors.bottomMargin: 1;
                    width: parent.width; height: 1; color: "white";
                }
            }

            // The default suggestion for Internet APNs, allowing the user
            // to copy from MMS contexts.
            ListItem.ItemSelector {
                id: copyFromMms
                visible: isInternet && mmsModel.count
                expanded: true
                model: 1
                selectedIndex: -1
                delegate: OptionSelectorDelegate {
                    text: i18n.tr("Copy from MMS APN")
                    showDivider: false
                }
                onDelegateClicked: {
                    suggestions.selectedIndex = -1;
                    parent.fill(mmsModel.get(0).qml);
                }

                // Dirty hack to remove unavoidable line under an ItemSelector.
                Rectangle {
                    anchors.bottom: parent.bottom; anchors.bottomMargin: 1;
                    width: parent.width; height: 1; color: "white";
                }
            }

            ListItem.ItemSelector {
                id: suggestions
                model: contextModel
                visible: contextModel.count
                expanded: true
                selectedIndex: -1
                delegate: OptionSelectorDelegate {
                    property string name: contextModel.get(index).qml.name
                    text: apnLib.isNameCustom(name) ? i18n.tr("Custom") : name
                    showDivider: false
                }
                onDelegateClicked: {
                    var context = model.get(index).qml;
                    copyFromMms.selectedIndex = -1;
                    copyFromInternet.selectedIndex = -1;
                    parent.fill(context);
                    suggestion = context;
                }
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

                function hasProtocol (link) {
                    return link.search(/^http[s]?\:\/\//) == -1;
                }

                function setHttp(link) {
                    if (hasProtocol(link)) {
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
                enabled: isValid && isChanged
                activeFocusOnPress: false

                onClicked: {

                    activated();

                    // Do we have a custom context?
                    var ctx = apnLib.getCustomContext(contextModel.type);

                    if (ctx) {
                        // We have a custom context we want to change.
                        // We cannot change it if it is active. If active,
                        // we need to defer changing it until it has been
                        // deactivated.
                        if(ctx.active) {
                            console.warn('Deactivating context before changing.');
                            ctx.disconnect();
                        }

                        console.warn('Changing context');
                        ctx.accessPointName = accessPointName.text
                        ctx.activeChanged.connect(apnLib.userTriedActivating.bind(ctx));
                        ctx.active = true;
                    } else {
                        // We will create a new APN.
                        waitForCreatedContextTimer.start();
                        apnLib.createContext(contextModel.type);
                    }
                }

                Timer {
                    id: waitForCreatedContextTimer
                    property int waited: 0
                    interval: 300
                    repeat: true
                    running: false
                    triggeredOnStart: true

                    onTriggered: {
                        var createdModel = apnLib.getCustomContext(contextModel.type);
                        if (createdModel) {
                            // The context should be deactivated here, but if
                            // not, we deactivate it.
                            if (createdModel.active) {
                                createdModel.disconnect();
                            }
                            createdModel.accessPointName = accessPointName.text;
                            createdModel.activeChanged.connect(apnLib.userTriedActivating.bind(createdModel));
                            createdModel.active = true;
                            running = false;
                            console.warn('Saw new model, trying to activate.');
                        } else {
                            console.warn('No created model yet...');
                        }

                        if (waited > 30) {
                            root.failed();
                        }
                        waited++;
                    }
                }

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
