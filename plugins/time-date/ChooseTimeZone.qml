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
    title: i18n.tr("Time Zone")

    UbuntuTimeDatePanel { id: timeDatePanel }

    TextField {
        anchors.top: parent.top
        anchors.margins: units.gu(2)
        width: parent.width
        id: filterCities
        onTextChanged: timeDatePanel.filter = text
    }

    ListView {
        anchors {
            top: filterCities.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        maximumFlickVelocity: height * 10
        flickDeceleration: height * 2

        model: timeDatePanel.timeZoneModel
        delegate: ListItem.Standard {
            text: displayName
            onClicked: timeDatePanel.timeZone = timeZone
        }
    }
}
