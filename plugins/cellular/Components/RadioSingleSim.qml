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
    height: childrenRect.height

    property alias selector: selector

    ListItem.ItemSelector {
        id: selector
        text: i18n.tr("Connection type:")
        showDivider: false
        expanded: true

        // an empty string is not a valid preference, which means
        // we disregard the interace and disable the selector
        enabled: sim.radioSettings.technologyPreference !== ""
        model: sim.radioSettings.modemTechnologies

        delegate: OptionSelectorDelegate {
            objectName: sim.path + "_radio_" + modelData
            text: {
                return {
                    'gsm': i18n.tr("2G only (saves battery)"),
                    'umts': i18n.tr("2G/3G (faster)"),
                    'lte': i18n.tr("2G/3G/4G (faster)")
                }[modelData]
            }
            showDivider: false
        }
        selectedIndex: model.indexOf(sim.radioSettings.technologyPreference)
        onDelegateClicked: {
            sim.radioSettings.technologyPreference = model[index];
        }
    }

}
