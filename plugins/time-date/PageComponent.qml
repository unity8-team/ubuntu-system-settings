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
import Ubuntu.Components.Popups 0.1
import Ubuntu.SystemSettings.TimeDate 1.0

ItemPage {
    title: i18n.tr("Time & Date")
    objectName: "timeDatePage"

    function getUTCOffset() {
        // We get the difference in minutes between UTC and our TZ (UTC - TZ)
        // but we want it in hours between our TZ and UTC (TZ - UTC), so divide
        // by -60 to invert and convert to hours.
        var offset = new Date().getTimezoneOffset() / -60
        var plus = offset >= 0 ? "+" : ""
        return "UTC" + plus + offset
    }

    UbuntuTimeDatePanel {
        id: timeDatePanel
        onTimeZoneChanged: {
            // Inform the JS engine that the TZ has been updated
            Date.timeZoneUpdated()
            timeZone.value = getUTCOffset()
        }
    }

    Column {
        anchors.fill: parent

        ListItem.Standard { text: i18n.tr ("Time zone:") }

        ListItem.SingleValue {
            objectName: "timeZone"
            id: timeZone
            //e.g. America/New_York -> America/New York
            text: timeDatePanel.timeZone.replace("_", " ")
            value: getUTCOffset()
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("ChooseTimeZone.qml"))
        }

        ListItem.ItemSelector {
            id: setTimeAutomatically
            objectName: "timeItemSelector"
            text: i18n.tr ("Set the time and date:")
            model: [ i18n.tr("Automatically") , i18n.tr("Manually")]
            expanded: true
            onSelectedIndexChanged: {
                var useNTP = (selectedIndex === 0) // 0 = Automatically
                timeDatePanel.useNTP = useNTP
            }
        }

        Binding {
            target: setTimeAutomatically
            property: "selectedIndex"
            value: timeDatePanel.useNTP ? 0 : 1
        }

        Timer {
            onTriggered: currentTime.text = Qt.formatDateTime(
                            new Date(),
                            Qt.DefaultLocaleLongDate)
            triggeredOnStart: true
            repeat: true
            running: true
        }

        Component {
            id: timePicker
            TimePicker {}
        }

        ListItem.Standard {
            id: currentTime
            objectName: "currentTime"
            progression: setTimeAutomatically.selectedIndex === 1 // Manually
            enabled: progression
            onClicked: {
                Qt.inputMethod.hide()
                var popupObj = PopupUtils.open(timePicker);
                popupObj.accepted.connect(
                            function(newDate) {
                                timeDatePanel.setTime(newDate.getTime() * 1000)
                })
            }
        }
    }
}
