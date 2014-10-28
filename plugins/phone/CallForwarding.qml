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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

ItemPage {

    objectName: "callForwardingPage"
    title: headerTitle
    property var sim
    property bool forwarding: sim.callForwarding.voiceUnconditional !== ""
    property string headerTitle: i18n.tr("Call forwarding")

    onForwardingChanged: {
        if (callForwardingSwitch.checked !== forwarding)
            callForwardingSwitch.checked = forwarding;
    }

    Connections {
        target: sim.callForwarding
        onVoiceUnconditionalChanged: {
            destNumberField.text = voiceUnconditional;
        }
        onVoiceUnconditionalComplete: {
            callForwardingIndicator.running = false;
            if (callForwardingSwitch.checked !== forwarding)
                callForwardingSwitch.checked = forwarding;
        }
    }

    Switch {
        id: callForwardingSwitch
        objectName: "callForwardingSwitch"
        checked: forwarding
        enabled: (forwarding === checked)
        visible: callForwardingItem.control === callForwardingSwitch
        onCheckedChanged: {
            if (!checked && forwarding) {
                callForwardingIndicator.running = true;
                sim.callForwarding.voiceUnconditional = "";
            }
        }
    }

    ActivityIndicator {
        id: callForwardingIndicator
        running: false
        visible: running
    }

    Column {
        anchors.fill: parent

        ListItem.Standard {
            id: callForwardingItem
            text: i18n.tr("Call forwarding")
            control: callForwardingIndicator.running ?
                         callForwardingIndicator : callForwardingSwitch
        }

        ListItem.Base {
            height: textItem.height + units.gu(2)
            Label {
                id: textItem
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }

                text: i18n.tr("Redirects phone calls to another number whenever you don't answer, or your phone is busy, turned off, or out of range.")
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
            showDivider: false
            visible: !callForwardingSwitch.checked
        }

        ListItem.Standard {
            id: forwardToItem
            text: i18n.tr("Forward to")
            visible: callForwardingSwitch.checked
            control: TextInput {
                id: destNumberField
                objectName: "destNumberField"
                horizontalAlignment: TextInput.AlignRight
                width: forwardToItem.width/2
                inputMethodHints: Qt.ImhDialableCharactersOnly
                text: sim.callForwarding.voiceUnconditional
                font.pixelSize: units.dp(18)
                font.weight: Font.Light
                font.family: "Ubuntu"
                color: "#AAAAAA"
                maximumLength: 20
                focus: true
                cursorVisible: text !== sim.callForwarding.voiceUnconditional ||
                               text === ""
                clip: true
                opacity: 0.9

                cursorDelegate: Rectangle {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: units.dp(1)
                    color: "#DD4814"
                    visible: destNumberField.cursorVisible
                }
            }
        }

        ListItem.Base {
            id: buttonsRowId
            Row {
                anchors.centerIn: parent
                spacing: units.gu(2)

                Button {
                    objectName: "cancel"
                    text: i18n.tr("Cancel")
                    width: (buttonsRowId.width-units.gu(2)*4)/3
                    enabled: !callForwardingIndicator.running
                    onClicked: {
                        destNumberField.text =
                                sim.callForwarding.voiceUnconditional;
                        if (forwarding !== callForwardingSwitch.checked)
                            callForwardingSwitch.checked = forwarding;
                    }
                }

                Button {
                    objectName: "set"
                    text: i18n.tr("Set")
                    width: (buttonsRowId.width-units.gu(2)*4)/3
                    enabled: !callForwardingIndicator.running
                    onClicked: {
                        callForwardingIndicator.running = true;
                        sim.callForwarding.voiceUnconditional = destNumberField.text;
                    }
                }
            }
            visible: callForwardingSwitch.checked &&
                     (destNumberField.text !==
                      sim.callForwarding.voiceUnconditional)
        }
    }
}
