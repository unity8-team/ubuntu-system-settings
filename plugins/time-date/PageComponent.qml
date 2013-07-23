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
import Ubuntu.SystemSettings.TimeDate 1.0

ItemPage {
    title: i18n.tr("Time & Date")

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
            id: timeZone
            //e.g. America/New_York -> America/New York
            text: timeDatePanel.timeZone.replace("_", " ")
            value: getUTCOffset()
            progression: true
        }

        ListItem.Divider {}

        ListItem.Standard {
            text: i18n.tr ("Set the time and date:")
        }

        ListItem.ValueSelector {
            id: setTimeAutomatically
            values: ["Automatically" , "Manually"]
            expanded: true
            // TODO: No way to have always expanded
            onExpandedChanged: expanded = true
        }

        Timer {
            onTriggered: currentTime.text = Qt.formatDateTime(
                            new Date(),
                            Qt.DefaultLocaleLongDate)
            triggeredOnStart: true
            repeat: true
            running: true
        }

        ListItem.Standard {
            id: currentTime
            progression: setTimeAutomatically.selectedIndex === 1 // Manually
            enabled: progression
        }
    }
}
