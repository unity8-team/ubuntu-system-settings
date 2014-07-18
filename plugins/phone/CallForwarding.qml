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
import MeeGo.QOfono 0.2

ItemPage {
    title: i18n.tr("Call forwarding")
    property bool canCheckForwarding: true
    property bool forwarding: callForwarding.voiceUnconditional !== ""
    property string modem

    onForwardingChanged: {
        console.warn("onForwardingChanged: " + forwarding);
        callForwardingSwitch.checked = forwarding;
    }

    OfonoCallForwarding {
        id: callForwarding
        modemPath: modem
        onVoiceUnconditionalChanged: {
            console.warn ("voiceUnconditionalChanged: " + voiceUnconditional);
        }
        onVoiceUnconditionalComplete: {
            console.warn ("voiceUnconditionalComplete: " + success);
            callForwardingIndicator.running = false;
            if (success)
                forwardToItem.control = contactLabel;
        }
    }

    Switch {
        id: callForwardingSwitch
        checked: forwarding
        enabled: (forwarding === checked)
        visible: callForwardingItem.control === callForwardingSwitch
        onCheckedChanged: {
            console.warn("onCheckedChanged: " + callForwarding.voiceUnconditional);
            if (!checked) {
                callForwarding.voiceUnconditional = "";
            }
        }
    }

    ActivityIndicator {
        id: callForwardingIndicator
        running: false
        visible: running
        onRunningChanged: console.warn("onRunningChanged: " + running)
    }

    Column {
        anchors.fill: parent

        ListItem.Standard {
            id: callForwardingItem
            text: i18n.tr("Call forwarding")
            control: callForwardingIndicator.running ? callForwardingIndicator : callForwardingSwitch
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

        ListItem.Base {
            Label {
                id: errorTextItem
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }

                text: i18n.tr("Call forwarding status can’t be checked right now. Try again later.")
                color: "red" // TODO: replace by the standard 'error color' if we get one in the toolkit
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
            showDivider: false
            visible: !callForwardingSwitch.checked && !canCheckForwarding
        }

        ListItem.Standard {
            id: forwardToItem
            property string contactName: ""
            text: i18n.tr("Forward to")
            control: (forwarding !== callForwardingSwitch.checked) ? destNumberField : contactLabel
            visible: callForwardingSwitch.checked
            Label {
                id: contactLabel
                text: callForwarding.voiceUnconditional
                visible: forwardToItem.control !== destNumberField
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (forwardToItem.control === contactLabel)
                            forwardToItem.control = destNumberField;
                    }
                }
            }

            TextField {
                id: destNumberField
                inputMethodHints: Qt.ImhDialableCharactersOnly
                text: callForwarding.voiceUnconditional
                visible: forwardToItem.control !== contactLabel
            }
        }

        ListItem.Base {
            id: buttonsRowId
            Row {
                anchors.centerIn: parent
                spacing: units.gu(2)

                Button {
                    text: i18n.tr("Cancel")
                    width: (buttonsRowId.width-units.gu(2)*4)/3
                    onClicked: {
                        destNumberField.text = callForwarding.voiceUnconditional;
                        if (forwarding !== callForwardingSwitch.checked)
                            callForwardingSwitch.checked = forwarding;
                        forwardToItem.control = contactLabel;
                    }
                }

                Button {
                    text: i18n.tr("Set")
                    width: (buttonsRowId.width-units.gu(2)*4)/3
                    onClicked: {
                        console.warn(destNumberField.text);
                        callForwardingIndicator.running = true;
                        callForwarding.voiceUnconditional = destNumberField.text;
                    }
                }
            }
            visible: callForwardingSwitch.checked
        }
    }
}
