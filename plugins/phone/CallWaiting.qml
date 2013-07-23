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

ItemPage {
    title: i18n.tr("Call waiting")

    /* Simulate going off and retreiving the status, TODO: replace by real data */
    Timer {
        id: callWaitingTimer
        interval: 3000
        running: false
    }
    Component.onCompleted: callWaitingTimer.start()

    Switch {
        id: callWaitingSwitch
        checked: false
        visible: callWaitingItem.control == callWaitingSwitch
    }

    ActivityIndicator {
        id: callWaitingIndicator
        running: true
        visible: callWaitingItem.control == callWaitingIndicator
    }

    Column {
        anchors.fill: parent

        ListItem.Standard {
            id: callWaitingItem
            text: i18n.tr("Call waiting")
            control: callWaitingTimer.running ? callWaitingIndicator : callWaitingSwitch
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

                text: i18n.tr("Lets you answer or start a new call while on another call, and switch between them")
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
            showDivider: false
            visible: !callDiversionSwitch.checked
        }
    }
}
