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
    title: i18n.tr("Time zone")

    UbuntuTimeDatePanel { id: timeDatePanel }

    ListItem.ItemSelector {
        id: setTimeZoneSelector
        text: i18n.tr("Set the time zone:")
        model: [i18n.tr("Automatically"), i18n.tr("Manually")]
        selectedIndex: 1 // TODO: get value once we have a working backend
        expanded: true
        visible: showAllUI
    }

    ListItem.Standard {
        anchors.top: setTimeZoneSelector.bottom
        text: timeDatePanel.timeZone
        enabled: false
        visible: showAllUI && setTimeZoneSelector.selectedIndex == 0 // Automatically
    }

    TextField {
        anchors {
            top: showAllUI ? setTimeZoneSelector.bottom : parent.top
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }
        id: filterCities
        onTextChanged: timeDatePanel.filter = text
        visible: setTimeZoneSelector.selectedIndex == 1 // Manually
        Component.onCompleted: forceActiveFocus()
        Connections {
            target: setTimeZoneSelector
            onSelectedIndexChanged: {
                if (setTimeZoneSelector.selectedIndex == 1)
                    filterCities.forceActiveFocus()
            }
        }
    }

    ListView {
        id: locationsListView
        clip: true
        anchors {
            top: filterCities.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        property string show: ""

        model: timeDatePanel.timeZoneModel
        visible: setTimeZoneSelector.selectedIndex == 1 && count > 0
        delegate: ListItem.Standard {
            text: displayName
            onClicked: {
                locationsListView.show = displayName
                timeDatePanel.timeZone = timeZone
            }
            selected: locationsListView.show === "" ?
                          timeDatePanel.timeZone == timeZone :
                          locationsListView.show == displayName
        }
    }

    Label {
        anchors.centerIn: parent
        visible: setTimeZoneSelector.selectedIndex ==1 &&
                 locationsListView.count == 0
        text: (filterCities.length == 0) ? i18n.tr("Enter your current location.") : i18n.tr("No matching place")
    }
}
