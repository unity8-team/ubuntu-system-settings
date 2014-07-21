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
    height: childrenRect.height

    property var sim1
    property var selector: selector

    ListItem.ItemSelector {
        id: selector
        objectName: "technologyPreferenceSelector"
        text: i18n.tr("Cellular data:")
        expanded: true
        enabled: sim1.radioSettings.technologyPreference !== ""
        model: [i18n.tr("Off"), i18n.tr("2G only (saves battery)"), i18n.tr("2G/3G/4G (faster)")]
        selectedIndex: DataHelpers.singleSimKeyToIndex(sim1.radioSettings.technologyPreference)
    }

    ListItem.Standard {
        id: dataRoamingItem
        objectName: "dataRoamingSwitch"
        text: i18n.tr("Data roaming")
        enabled: sim1.connMan.powered
        control: Switch {
            id: dataRoamingControl
            checked: sim1.connMan.roamingAllowed
            onClicked: sim1.connMan.roamingAllowed = checked
        }
    }

    Connections {
        target: sim1.connMan
        onPoweredChanged: {
            if (powered) {
                selector.selectedIndex = DataHelpers.singleSimKeyToIndex(sim1.radioSettings.technologyPreference);
            } else {
                selector.selectedIndex = 0;
            }
        }
    }

    Connections {
        target: sim1.radioSettings
        onTechnologyPreferenceChanged: {
            if (connMan.powered) {
                selector.selectedIndex = DataHelpers.singleSimKeyToIndex(preference);
            }
        }
    }

    Binding {
        target: sim1.connMan
        property: "powered"
        value: selector.selectedIndex !== 0
    }

    Binding {
        target: sim1.radioSettings
        property: "technologyPreference"
        value: {
            var i = selector.selectedIndex;
            if (i === 1) {
                return 'gsm';
            } else if (i === 2) {
                return 'any';
            } else {
                return sim1.radioSettings.technologyPreference
            }
        }
    }
}
