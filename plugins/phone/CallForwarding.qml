/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Sebastien Bacher <sebastien.bacher@canonical.com>
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
import SystemSettings 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Themes.Ambiance 0.1

ItemPage {
    id: page
    objectName: "callForwardingPage"
    title: headerTitle
    property var sim
    property bool forwarding: sim.callForwarding.voiceUnconditional !== ""
    property string headerTitle: i18n.tr("Call forwarding")
    property real faded: 0.6

    // Holds the forwarding item we're currently editing.
    property var editing: null

    states: [
        // State {
        //     name: "forwardCheck"
        //     PropertyChanges { target: fwdAll; opacity: faded; control: allAct }
        //     PropertyChanges { target: fwdSomeTitle; opacity: faded }

        //     PropertyChanges { target: fwdBusy; opacity: faded; control: busyAct }

        //     PropertyChanges { target: fwdLost; opacity: faded; control: lostAct }

        //     PropertyChanges { target: fwdUnreachable; opacity: faded; control: unreachableAct }
        // },
        // State {
        //     name: "forwardFailed"
        //     PropertyChanges { target: fwdSomeTitle; opacity: faded }
        //     PropertyChanges { target: fwdFailedLabel; visible: true }

        //     PropertyChanges { target: fwdAll; failed: true }
        //     PropertyChanges { target: fwdBusy; failed: true }
        //     PropertyChanges { target: fwdLost; failed: true }
        //     PropertyChanges { target: fwdUnreachable; failed: true }
        // },
        State {
            name: "forwardAll"
            PropertyChanges { target: fwdSomeTitle; opacity: faded }
            PropertyChanges { target: fwdBusy; opacity: faded; enabled: false }
            PropertyChanges { target: fwdLost; opacity: faded; enabled: false }
            PropertyChanges { target: fwdUnreachable; opacity: faded; enabled: false }
            when: fwdAll.clientRule && fwdAll.clientRule !== ""
        }
        // State {
        //     name: "forwardSome"
        //     // PropertyChanges { target: fwdAll; opacity: faded; }
        //     // PropertyChanges { target: fwdAllCheck; enabled: false }
        // }
    ]

    // We need to disable keyboard anchoring because we implement the
    // KeyboardRectangle pattern
    Binding {
        target: main
        property: "anchorToKeyboard"
        value: false
    }

    onEditingChanged: {
        console.warn('onEditingChanged', editing);
        [fwdAll, fwdBusy, fwdLost, fwdUnreachable].forEach(function (n) {
            if (editing) {
                if (n !== editing) {
                    n.enabled = false;
                }
            } else {
                n.enabled = true;
            }
        });
    }

    flickable: null
    Flickable {
        id: flick

        // this is necessary to avoid the page to appear below the header
        clip: true
        flickableDirection: Flickable.VerticalFlick
        anchors {
            fill: parent
            bottomMargin: keyboardButtons.height + keyboard.height
        }
        contentHeight: contents.height + units.gu(2)
        contentWidth: parent.width

        Column {
            id: contents
            anchors { left: parent.left; right: parent.right }
            spacing: units.gu(1)

            CallForwardItem {
                id: fwdAll
                anchors { left: parent.left; right: parent.right }
                ruleName: "voiceUnconditional"
                callForwarding: sim.callForwarding
                text: i18n.tr("Forward every incoming call")
                onEditing: page.editing = fwdAll
                onStoppedEditing: page.editing = null
                Component.onCompleted: console.warn(ruleName)
            }

            Label {
                id: fwdAllCaption
                anchors {
                    left: parent.left; right: parent.right; margins: units.gu(1);
                }
                fontSize: "small"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: i18n.tr("Redirects all phone calls to another number.")
                opacity: 0.8
            }

            Label {
                id: fwdFailedLabel
                visible: false
                text: i18n.tr("Call forwarding status can’t be checked " +
                              "now. Try again later.")
                color: UbuntuColors.red
                horizontalAlignment: Text.AlignHCenter
            }

            SettingsItemTitle {
                id: fwdSomeTitle
                text: i18n.tr("Forward incoming calls when:")
                showDivider: false
            }

            CallForwardItem {
                id: fwdBusy
                anchors { left: parent.left; right: parent.right }
                ruleName: "voiceBusy"
                callForwarding: sim.callForwarding
                text: i18n.tr("I’m on another call")
                onEditing: page.editing = fwdBusy
                onStoppedEditing: page.editing = null
            }

            CallForwardItem {
                id: fwdLost
                anchors { left: parent.left; right: parent.right }
                ruleName: "voiceNoReply"
                callForwarding: sim.callForwarding
                text: i18n.tr("I don’t answer")
                onEditing: page.editing = fwdLost
                onStoppedEditing: page.editing = null
            }

            CallForwardItem {
                id: fwdUnreachable
                anchors { left: parent.left; right: parent.right }
                ruleName: "voiceNotReachable"
                callForwarding: sim.callForwarding
                text: i18n.tr("My phone is unreachable")
                onEditing: page.editing = fwdUnreachable
                onStoppedEditing: page.editing = null
            }
        }
    } // Flickable

    Rectangle {
        id: keyboardButtons
        anchors {
            left: parent.left
            right: parent.right
            bottom: keyboard.top
        }
        color: Theme.palette.selected.background
        visible: editing !== null
        height: units.gu(6)
        Button {
            id: kbdContacts
            anchors {
                left: parent.left
                leftMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
            }
            text: i18n.tr("Contacts…")
        }

        Button {
            id: kbdCancel
            anchors {
                right: kbdSet.left
                rightMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
            }
            text: i18n.tr("Cancel")
            onClicked: editing.cancel()
        }

        Button {
            id: kbdSet
            anchors {
                right: parent.right
                rightMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
            }
            text: i18n.tr("Set")
            onClicked: editing.setRule()
        }
    }

    KeyboardRectangle {
        id: keyboard
        anchors.bottom: parent.bottom
        onHeightChanged: {
            console.warn('onHeightChanged...');
        }
    }

    // onForwardingChanged: {
    //     if (callForwardingSwitch.checked !== forwarding)
    //         callForwardingSwitch.checked = forwarding;
    // }

    // Connections {
    //     target: sim.callForwarding
    //     onVoiceUnconditionalChanged: {
    //         destNumberField.text = voiceUnconditional;
    //     }
    //     onVoiceUnconditionalComplete: {
    //         callForwardingIndicator.running = false;
    //         if (callForwardingSwitch.checked !== forwarding)
    //             callForwardingSwitch.checked = forwarding;
    //     }
    // }

    // Column {
    //     anchors.fill: parent


    //     ListItem.Standard {
    //         id: forwardToItem
    //         text: i18n.tr("Forward to")
    //         visible: callForwardingSwitch.checked
    //         control: TextInput {
    //             id: destNumberField
    //             objectName: "destNumberField"
    //             horizontalAlignment: TextInput.AlignRight
    //             width: forwardToItem.width/2
    //             inputMethodHints: Qt.ImhDialableCharactersOnly
    //             text: sim.callForwarding.voiceUnconditional
    //             font.pixelSize: units.dp(18)
    //             font.weight: Font.Light
    //             font.family: "Ubuntu"
    //             color: "#AAAAAA"
    //             maximumLength: 20
    //             focus: true
    //             cursorVisible: text !== sim.callForwarding.voiceUnconditional ||
    //                            text === ""
    //             clip: true
    //             opacity: 0.9

    //             cursorDelegate: Rectangle {
    //                 anchors.top: parent.top
    //                 anchors.bottom: parent.bottom
    //                 width: units.dp(1)
    //                 color: "#DD4814"
    //                 visible: destNumberField.cursorVisible
    //             }
    //             onVisibleChanged:
    //                 if (visible === true) forceActiveFocus()
    //         }
    //     }

    //     ListItem.Base {
    //         id: buttonsRowId
    //         Row {
    //             anchors.centerIn: parent
    //             spacing: units.gu(2)

    //             Button {
    //                 objectName: "cancel"
    //                 text: i18n.tr("Cancel")
    //                 width: (buttonsRowId.width-units.gu(2)*4)/3
    //                 enabled: !callForwardingIndicator.running
    //                 onClicked: {
    //                     destNumberField.text =
    //                             sim.callForwarding.voiceUnconditional;
    //                     if (forwarding !== callForwardingSwitch.checked)
    //                         callForwardingSwitch.checked = forwarding;
    //                 }
    //             }

    //             Button {
    //                 objectName: "set"
    //                 text: i18n.tr("Set")
    //                 width: (buttonsRowId.width-units.gu(2)*4)/3
    //                 enabled: !callForwardingIndicator.running
    //                 onClicked: {
    //                     callForwardingIndicator.running = true;
    //                     sim.callForwarding.voiceUnconditional = destNumberField.text;
    //                 }
    //             }
    //         }
    //         visible: callForwardingSwitch.checked &&
    //                  (destNumberField.text !==
    //                   sim.callForwarding.voiceUnconditional)
    //     }
    // }


    Connections {
        target: sim.callForwarding

        onGetPropertiesFailed: {
            console.warn('failed');
            root.state = "forwardFailed";
        }

        onReadyChanged: {
            console.warn('ready');
        }

        // All.
        onVoiceUnconditionalChanged: {
            fwdAll.clientRule = property;
            console.warn('voiceUnconditional changed', property);
        }
        onVoiceUnconditionalComplete: {
            console.warn('voiceUnconditional complete', success);
            fwdAll.serverResponse(success);
        }

        // Busy.
        onVoiceBusyChanged: {
            console.warn('voiceBusy changed', property);
            fwdBusy.clientRule = property;
        }
        onVoiceBusyComplete: {
            console.warn('onVoiceBusyComplete complete', success);
            fwdBusy.serverResponse(success)
        }

        // Lost.
        onVoiceNoReplyChanged: {
            console.warn('voiceNoReply changed', property);
            fwdLost.clientRule = property;
        }
        onVoiceNoReplyComplete: {
            console.warn('onVoiceNoReplyComplete complete', success);
            fwdLost.serverResponse(success);
        }

        // Unreachable.
        onVoiceNotReachableChanged: {
            console.warn('voiceNotReachableChanged changed', property);
            fwdUnreachable.clientRule = property;
        }
        onVoiceNotReachableComplete: {
            console.warn('voiceNotReachableComplete complete', success);
            fwdUnreachable.serverResponse(success);
        }
    }
}
