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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import SystemSettings 1.0

ItemPage {
    title: i18n.tr("Call waiting")

    function setCallWaiting(checked) {
        timer.start();
    }

    // Simulate going off and retreiving the status
    Timer {
        property var dialogToClose
        id: callWaitingTimer
        interval: 5000
        running: false
    }

    Column {
        anchors.fill: parent
        spacing: units.gu(2)

        ListItem.Standard {
            id: callWaitingItem
            text: i18n.tr("Call waiting")
            control: timer.running ? callWaitingIndicator : callWaitingSwitch
        }

        Switch {
            id: callWaitingSwitch
            checked: false
            onCheckedChanged: setCallWaiting(checked)
            visible: callWaitingItem.control == callWaitingSwitch
        }

        ActivityIndicator {
            id: callWaitingIndicator
            running: true
            visible: callWaitingItem.control == callWaitingIndicator
        }

        Text {
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(3)
            }

            text: i18n.tr("Lets you answer or start a new call while on " +
                          "another call, and switch between them")
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
    }
}
