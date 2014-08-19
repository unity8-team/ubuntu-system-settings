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
    property var sim1
    property var sim2
    property var selector: selector
    property var prefMap: ['gsm', 'any']

    function getUsedSim () {
        if (state === "sim1") {
            return sim1;
        } else if (state === "sim2") {
            return sim2;
        } else {
            return null;
        }
    }

    height: childrenRect.height
    states: [
        State {
            name: "sim1"
            when: sim1.connMan.powered && !sim2.connMan.powered
        },
        State {
            name: "sim2"
            when: sim2.connMan.powered && !sim1.connMan.powered
        },
        State {
            name: "bothOnline"
            when: sim1.connMan.powered && sim2.connMan.powered
            StateChangeScript { script: {
                sim2.connMan.powered = false;
            }}
        }
    ]

    ListItem.ItemSelector {
        id: use
        objectName: "use"
        text: i18n.tr("Cellular data:")
        expanded: true
        model: ["Off", "sim1", "sim2"]
        delegate: OptionSelectorDelegate {
            objectName: "use" + modelData
            text: {
                var t;
                if (modelData === "sim1") {
                    t = sim1.title
                } else if (modelData === "sim2") {
                    t = sim2.title
                } else {
                    t = i18n.tr(modelData);
                }
                return t;
            }
        }
        selectedIndex: [true, sim1.connMan.powered, sim2.connMan.powered].lastIndexOf(true)
        onDelegateClicked: {
            sim1.connMan.powered = (index === 1)
            sim2.connMan.powered = (index === 2)
        }
    }

    Connections {
        target: sim1.connMan
        onPoweredChanged: {
            if (powered) {
                use.selectedIndex = 1;
            }
        }
    }

    Connections {
        target: sim2.connMan
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
        model: [i18n.tr("2G only (saves battery)"), i18n.tr("2G/3G/4G (faster)")]
        visible: use.selectedIndex !== 0
        onDelegateClicked: {
            var sim = getUsedSim();
            if (sim) {
                sim.radioSettings.technologyPreference = prefMap[index];
            }
        }
    }

    Connections {
        target: sim1.radioSettings
        onTechnologyPreferenceChanged: {
            if (sim1.connMan.powered) {
                selector.selectedIndex = DataHelpers.dualSimKeyToIndex(preference);
            }
        }
    }

    Connections {
        target: sim2.radioSettings
        onTechnologyPreferenceChanged: {
            if (sim2.connMan.powered) {
                selector.selectedIndex = DataHelpers.dualSimKeyToIndex(preference);
            }
        }
    }

    Binding {
        target: selector
        property: "enabled"
        value: getUsedSim() && (getUsedSim().radioSettings.technologyPreference !== "")
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
