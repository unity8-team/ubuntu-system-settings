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
    id: root
    property var selector: selector
    property var prefMap: ['gsm', 'umts']

    function getNameFromIndex (index) {
        return [i18n.tr("Off"), sims[0].title, sims[1].title][index];
    }

    function getUsedSim () {
        if (state === "sim1Online") {
            return sims[0];
        } else if (state === "sim2Online") {
            return sims[1];
        } else {
            return null;
        }
    }

    height: childrenRect.height
    states: [
        State {
            name: "sim1Online"
            when: sims[0].connMan.powered && !sims[1].connMan.powered
            StateChangeScript { script: {
                selector.selectedIndex =
                    DataHelpers.dualSimKeyToIndex(
                        sims[0].radioSettings.technologyPreference)
            }}
        },
        State {
            name: "sim2Online"
            when: sims[1].connMan.powered && !sims[0].connMan.powered
            StateChangeScript { script: {
                selector.selectedIndex =
                    DataHelpers.dualSimKeyToIndex(
                        sims[1].radioSettings.technologyPreference)
            }}
        },
        State {
            name: "bothOnline"
            when: sims[0].connMan.powered && sims[1].connMan.powered
            StateChangeScript { script: {
                sims[1].connMan.powered = false;
            }}
        }
    ]

    ListItem.ItemSelector {
        id: use
        objectName: "use"
        text: i18n.tr("Cellular data:")
        expanded: true
        model: ["off", "sim1", "sim2"]
        delegate: OptionSelectorDelegate {
            objectName: "use" + modelData
            text: getNameFromIndex(index)
        }
        selectedIndex: [true, sims[0].connMan.powered, sims[1].connMan.powered]
            .lastIndexOf(true)
        onDelegateClicked: {
            sims[0].connMan.powered = (index === 1)
            sims[1].connMan.powered = (index === 2)
        }
    }

    Connections {
        target: sims[0].connMan
        onPoweredChanged: {
            if (powered) {
                use.selectedIndex = 1;
            }
        }
    }

    Connections {
        target: sims[1].connMan
        onPoweredChanged: {
            if (powered) {
                use.selectedIndex = 2;
            }
        }
    }

    ListItem.ItemSelector {
        id: selector
        objectName: "technologyPreferenceSelector"
        expanded: true
        model:
            [i18n.tr("2G only (saves battery)"), i18n.tr("2G/3G/4G (faster)")]
        visible: use.selectedIndex !== 0
        onDelegateClicked: {
            var sim = getUsedSim();
            if (sim) {
                sim.radioSettings.technologyPreference = prefMap[index];
            }
        }
    }

    Connections {
        target: sims[0].radioSettings
        onTechnologyPreferenceChanged: {
            if (sims[0].connMan.powered) {
                selector.selectedIndex =
                    DataHelpers.dualSimKeyToIndex(preference);
            }
        }
    }

    Connections {
        target: sims[1].radioSettings
        onTechnologyPreferenceChanged: {
            if (sims[1].connMan.powered) {
                selector.selectedIndex =
                    DataHelpers.dualSimKeyToIndex(preference);
            }
        }
    }

    Binding {
        target: selector
        property: "enabled"
        value: getUsedSim() &&
            (getUsedSim().radioSettings.technologyPreference !== "")
        when: getUsedSim()
    }

    ListItem.Standard {
        id: dataRoamingItem
        objectName: "dataRoamingSwitch"
        text: i18n.tr("Data roaming")
        enabled: use.selectedIndex !== 0
        control: Switch {
            id: dataRoamingControl
            onClicked: getUsedSim().connMan.roamingAllowed = checked
        }
    }

    Binding {
        target: dataRoamingControl
        property: "checked"
        value: getUsedSim() && getUsedSim().connMan.roamingAllowed
        when: getUsedSim()
    }
}
