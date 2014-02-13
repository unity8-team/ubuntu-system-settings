/*
 * Copyright (C) 2013 Michael Zanetti <michael_zanetti@gmx.net>
 *               2013 Canonical Ltd
 * Canonical modifications by Iain Lane <iain.lane@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.Pickers 0.1

Dialog {
    id: root
    title: i18n.tr("Set time & date")

    signal accepted(date date)
    signal rejected

    property date date

    onDateChanged: {
        currentDate.value = Qt.formatDate(date, "d MMMM yyyy")
        currentTime.value = Qt.formatTime(date, "h:mm:ssAP")
    }

    Timer {
        id: timer
        onTriggered: date = new Date()
        triggeredOnStart: true
        repeat: true
        running: true
    }

    Row {
        ListItem.SingleValue {
            id: currentTime
            property alias date: root.date
            property Item picker

            text: "Time"
            showDivider: false
            onClicked: {
                timer.running = false
                picker = PickerPanel.openDatePicker(currentTime,
                                                   "date",
                                                   "Hours|Minutes|Seconds")
            }
        }

        Connections {
            target: currentTime.picker
            onClosed: buttonSet.enabled = true
        }
    }

    Row {
        ListItem.SingleValue {
            id: currentDate
            property alias date: root.date
            property Item picker

            text: "Date"
            showDivider: false
            onClicked: {
                timer.running = false
                picker = PickerPanel.openDatePicker(currentDate,
                                                   "date",
                                                   "Years|Months|Days")
            }
        }

        Connections {
            target: currentTime.picker
            onClosed: buttonSet.enabled = true
        }
    }

    Row {
        spacing: units.gu(1)
        Button {
            text: i18n.tr("Cancel")
            onClicked: {
                root.rejected()
                PopupUtils.close(root)
            }
            width: (parent.width - parent.spacing) / 2
        }
        Button {
            id: buttonSet
            objectName: "TimePickerOKButton"
            text: i18n.tr("Set")
            enabled: false

            onClicked: {
                root.accepted(root.date)
                PopupUtils.close(root)
            }
            width: (parent.width - parent.spacing) / 2
        }
    }
}
