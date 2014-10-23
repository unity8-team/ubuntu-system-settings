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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Column {

    objectName: "noSim"

    ListItem.Standard {
        text: i18n.tr("Cellular data:")
        enabled: false
        control: Switch {
            checked: false
        }
    }

    ListItem.Standard {
        text: i18n.tr("Data roaming")
        enabled: false
        showDivider: false
        control: Switch {
            checked: false
        }
    }

    ListItem.Divider {}

    ListItem.SingleValue {
        text: i18n.tr("Carrier");
        value: i18n.tr("N/A")
        enabled: false
    }
}
