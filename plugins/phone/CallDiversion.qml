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
    title: i18n.tr("Call diversion")
    property bool canCheckDiversion: true

    /* Simulate going off and retreiving the status, TODO: replace by real data */
    Timer {
        id: callDiversionTimer
        interval: 3000
        running: false
        onTriggered: canCheckDiversion = false
    }
    Component.onCompleted: callDiversionTimer.start()

    Switch {
        id: callDiversionSwitch
        checked: false
        visible: callDiversionItem.control == callDiversionSwitch
    }

    ActivityIndicator {
        id: callDiversionIndicator
        running: true
        visible: callDiversionItem.control == callDiversionIndicator
    }

    Label {
        id: contactLabel
        text: "Not working yet"
    }

    Column {
        anchors.fill: parent

        ListItem.Standard {
            id: callDiversionItem
            text: i18n.tr("Call diversion")
            control: callDiversionTimer.running ? callDiversionIndicator : callDiversionSwitch
        }

        ListItem.Base {
            height: textItem.height + units.gu(2)
            Text {
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
            visible: !callDiversionSwitch.checked
        }

        ListItem.Base {
            Text {
                id: errorTextItem
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }

                text: i18n.tr("Call diversion status can’t be checked right now. Try again later.")
                color: "red" // TODO: replace by the standard 'error color' if we get one in the toolkit
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
            showDivider: false
            visible: !callDiversionSwitch.checked && !canCheckDiversion
        }

        ListItem.Standard {
            property string contactName: ""
            text: i18n.tr("Divert to")
            control: contactLabel
            visible: callDiversionSwitch.checked
        }

        ListItem.Base {
            id: buttonsRowId
            Row {
                anchors.centerIn: parent
                spacing: units.gu(2)
                Repeater {
                    model: [i18n.tr("Contacts…"), i18n.tr("Cancel"), i18n.tr("Set") ]
                    Button {
                        text: modelData
                        width: (buttonsRowId.width-units.gu(2)*4)/3
                    }
                }
            }
            visible: callDiversionSwitch.checked
        }
    }
}
