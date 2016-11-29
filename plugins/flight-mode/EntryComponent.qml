/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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

import QtQuick 2.4
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.FlightMode 1.0 as FlightMode
import Ubuntu.Settings.Components 0.1 as USC


SettingsListItems.Icon {
    text: i18n.tr(model.displayName)
    iconSource: model.icon

    Switch {
        id: switchItem

        SlotsLayout.position: SlotsLayout.Trailing
        USC.ServerPropertySynchroniser {
            userTarget: switchItem
            userProperty: "checked"
            serverTarget: helper
            serverProperty: "inFlightMode"

            onSyncTriggered: helper.setFlightMode(value)
        }

        FlightMode.Helper {
            id: helper
        }
    }
}
