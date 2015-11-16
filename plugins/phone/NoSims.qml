/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Ken Vandine <ken.vandine@canonical.com>
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/
import QtQuick 2.4
import SystemSettings.ListItems 1.0 as SettingsListItems

Column {

    SettingsListItems.StandardProgression {
        text: i18n.tr("Call forwarding")
        enabled: false
    }

    SettingsListItems.StandardProgression {
        text: i18n.tr("Call waiting")
        enabled: false
    }

    SettingsListItems.StandardProgression {
        text: i18n.tr("Services")
        enabled: false
    }
}
