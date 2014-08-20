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

    property var selector: selector

    ListItem.ItemSelector {
        id: selector
        objectName: "technologyPreferenceSelector"
        text: i18n.tr("Cellular data:")
        expanded: true
        enabled: sim.radioSettings.technologyPreference !== ""
        model: {
            var m = sim.radioSettings.modemTechnologies.slice(0);
            m.unshift("off");
            return m;
        }
        delegate: OptionSelectorDelegate {
            text: function () {
                return {
                    'off': i18n.tr("Off"),
                    'gsm': i18n.tr("2G only (saves battery)"),
                    'umts': i18n.tr("2G/3G (faster)"),
                    'lte': i18n.tr("2G/3G/4G (faster)")
                }()[modelData]
            }
        }
        selectedIndex: {
            if (sim.connMan.powered) {
                return DataHelpers.singleSimKeyToIndex(
                    sim.radioSettings.technologyPreference);
            } else {
                return 0;
            }
        }
    }

    ListItem.Standard {
        id: dataRoamingItem
        objectName: "dataRoamingSwitch"
        text: i18n.tr("Data roaming")
        enabled: sim.connMan.powered
        control: Switch {
            id: dataRoamingControl
            checked: sim.connMan.roamingAllowed
            onClicked: sim.connMan.roamingAllowed = checked
        }
    }

    Connections {
        target: sim.connMan
        onPoweredChanged: {
            if (powered) {
                selector.selectedIndex = DataHelpers.singleSimKeyToIndex(
                    sim.radioSettings.technologyPreference);
            } else {
                selector.selectedIndex = 0;
            }
        }
    }

    Connections {
        target: sim.radioSettings
        onTechnologyPreferenceChanged: {
            var selIndex = selector.selectedIndex;
            if (selIndex > 0) {
                sim.radioSettings.technologyPreference =
                    DataHelpers.singleSimIndexToKey(selIndex);
            }
        }
    }

    Binding {
        target: sim.connMan
        property: "powered"
        value: selector.selectedIndex !== 0
    }

    Binding {
        target: sim.radioSettings
        property: "technologyPreference"
        value: {
            var i = selector.selectedIndex;
            if (i === 1) {
                return 'gsm';
            } else if (i === 2) {
                return 'umts';
            } else {
                return sim.radioSettings.technologyPreference
            }
        }
    }
}
