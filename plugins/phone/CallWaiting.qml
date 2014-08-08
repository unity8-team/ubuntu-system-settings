/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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
    objectName: "callWaitingPage"
    title: headerTitle
    property var sim
    property string headerTitle: i18n.tr("Call waiting")

    Connections {
        target: sim.callSettings
        onVoiceCallWaitingChanged: {
            callWaitingIndicator.running = false;
        }
    }

    ActivityIndicator {
        id: callWaitingIndicator
        running: true
        visible: running
    }

    Switch {
        id: callWaitingSwitch
        objectName: "callWaitingSwitch"
        visible: !callWaitingIndicator.running
        checked: sim.callSettings.voiceCallWaiting !== "disabled"
        onClicked: {
            callWaitingIndicator.running = true;
            if (checked)
                sim.callSettings.voiceCallWaiting = "enabled";
            else
                sim.callSettings.voiceCallWaiting = "disabled";
        }
    }

    Column {
        anchors.fill: parent

        ListItem.Standard {
            id: callWaitingItem
            text: i18n.tr("Call waiting")
            control: callWaitingIndicator.running ?
                     callWaitingIndicator : callWaitingSwitch
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

                text: i18n.tr("Lets you answer or start a new call while on another call, and switch between them")
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
            showDivider: false
        }
    }
}
