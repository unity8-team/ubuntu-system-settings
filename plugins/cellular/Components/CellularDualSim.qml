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
import "data-helpers.js" as DataHelpers

Column {

    id: ds

    height: childrenRect.height

    property var sim1
    property var sim2
    property alias useSim: ds.sim1
    property var selector: selector
    property var prefMap: ['gsm', 'any']

    ListItem.ItemSelector {
        id: use
        objectName: "use"
        text: i18n.tr("Cellular data:")
        expanded: true
        model: [i18n.tr("Off"), sim1.title, sim2.title]
        // selectedIndex: [true, sim1.connMan.powered, sim2.connMan.powered].lastIndexOf(true);
    }

    Binding {
        target: useSim.connMan
        property: "powered"
        value: useSim === sim1 ? use.selectedIndex === 1 : use.selectedIndex === 2
    }

    ListItem.ItemSelector {
        id: selector
        objectName: "technologyPreferenceSelector"
        expanded: true
        model: [i18n.tr("2G only (saves battery)"), i18n.tr("2G/3G/4G (faster)")]

        // enabled: useSim.radioSettings.technologyPreference !== ""
        // selectedIndex: prefMap.indexOf(useSim.radioSettings.technologyPreference)

        //onDelegateClicked: DataHelpers.dualTechSelectorClicked(index)
    }


    Binding {
        target: useSim.radioSettings
        property: "technologyPreference"
        value: prefMap[selector.selectedIndex]
    }

    ListItem.Standard {
        id: dataRoamingItem
        objectName: "dataRoamingSwitch"
        text: i18n.tr("Data roaming")
        enabled: use.selectedIndex !== 0
        control: Switch {
            id: dataRoamingControl
            checked: useSim.connMan.roamingAllowed
            onClicked: useSim.connMan.roamingAllowed = checked
        }
    }

}
