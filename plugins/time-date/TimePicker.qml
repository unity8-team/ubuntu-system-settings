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

Dialog {
    id: root
    title: i18n.tr("Set time & date")

    property alias hour: hourScroller.currentIndex
    property alias minute: minuteScroller.currentIndex
    property alias seconds: secondScroller.currentIndex
    // 1 - 31
    property int day: priv.now.getDate() - 1
    // 0 - 11
    property alias month: monthScroller.currentIndex
    property int year: priv.now.getFullYear()
    property alias minYear: yearScroller.min
    property alias maxYear: yearScroller.max

    signal accepted(int hours, int minutes, int seconds,
                    int day, int month, int year)
    signal rejected

    QtObject {
        id: priv
        property date now: new Date()

        function getDays(month, year) {
            switch(month) {
            case 1:
                if (((year % 4 === 0) &&
                     (year % 100 !== 0))
                        || (year % 400 === 0)) {
                    return 29;
                }
                return 28;
            case 3:
            case 5:
            case 8:
            case 10:
                return 30;
            default:
                return 31;
            }
        }
    }

    Label {
        text: i18n.tr("Time")
    }
    Row {
        height: units.gu(17)

        Scroller {
            id: hourScroller
            objectName: "hourScroller"
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: parent.width / 3
            labelText: i18n.tr("Hour")

            min: 00
            max: 23
            currentIndex: priv.now.getHours()
        }
        Scroller {
            id: minuteScroller
            objectName: "minuteScroller"
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: parent.width / 3
            labelText: i18n.tr("Minute")

            min: 00
            max: 59
            currentIndex: priv.now.getMinutes()
        }
        Scroller {
            id: secondScroller
            objectName: "secondScroller"
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: parent.width / 3
            labelText: i18n.tr("Second")

            min: 00
            max: 59
            currentIndex: priv.now.getSeconds()
        }
    }

    Label {
        text: i18n.tr("Date")
    }

    Row {
        height: units.gu(17)
        Scroller {
            id: dayScroller
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: parent.width / 3
            labelText: i18n.tr("Day")
            min: 1
            max: priv.getDays(root.month,
                              root.year)
            currentIndex: priv.now.getDate() - 1
            onCurrentIndexChanged: root.day = currentIndex + 1
        }
        Scroller {
            id: monthScroller
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: parent.width / 3
            labelText: i18n.tr("Month")
            currentIndex: priv.now.getMonth()

            model: [
                  i18n.tr("January")
                , i18n.tr("February")
                , i18n.tr("March")
                , i18n.tr("April")
                , i18n.tr("May")
                , i18n.tr("June")
                , i18n.tr("July")
                , i18n.tr("August")
                , i18n.tr("September")
                , i18n.tr("October")
                , i18n.tr("November")
                , i18n.tr("December")]

        }
        Scroller {
            id: yearScroller
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: parent.width / 3
            labelText: i18n.tr("Year")
            min: 1970
            max: 2048
            currentIndex: priv.now.getFullYear() - min
            onCurrentIndexChanged: root.year = currentIndex + min
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
            objectName: "TimePickerOKButton"
            text: i18n.tr("Set")

            onClicked: {
                root.accepted(root.hour, root.minute, root.seconds,
                              root.day, root.month, root.year)
                PopupUtils.close(root)
            }
            width: (parent.width - parent.spacing) / 2
        }
    }
}
