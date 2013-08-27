/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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

import GSettings 1.0
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

ListItem.Standard {
    id: root
    icon: Qt.resolvedUrl(model.icon)
    iconFrame: false
    text: i18n.tr(model.displayName)
    control: Switch {
        id: control
        checked: networkSettings.flightMode
        onCheckedChanged: networkSettings.flightMode = checked
        enabled: false /* TODO: enable when there is a flight mode backend */

        GSettings {
            id: networkSettings
            schema.id: "com.ubuntu.touch.network"
            onChanged: {
                if (key == "flightMode")
                    control.checked = value
            }
        }
    }
}
