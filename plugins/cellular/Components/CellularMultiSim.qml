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
    id: root
    property var selector: selector

    function getNameFromIndex (index) {
        if (index === 0) {
            return i18n.tr("Off");
        } else if (index > 0) {
            return sims[index - 1].title;
        }
    }

    function getOnlineSim () {
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
                console.warn('sim1Online');
                selector.selectedIndex =
                    selector.model.indexOf(
                        sim1.radioSettings.technologyPreference)
            }}
        },
        State {
            name: "sim2Online"
            when: sims[1].connMan.powered && !sims[0].connMan.powered
            StateChangeScript { script: {
                console.warn('sim2Online');
                selector.selectedIndex =
                    selector.model.indexOf(
                        sim2.radioSettings.technologyPreference)
            }}
        },
        State {
            name: "bothOnline"
            when: sims[0].connMan.powered && sims[1].connMan.powered
            StateChangeScript { script: {
                console.warn('both sims online');
                sims[1].connMan.powered = false;
            }}
        }
    ]

    ListItem.ItemSelector {
        id: use
        objectName: "use"
        text: i18n.tr("Cellular data:")
        expanded: true
        model: {
            // create a model of 'off' and all sim paths
            var m = ['off'];
            sims.forEach(function (sim) {
                m.push(sim.path);
            });
            return m;
        }
        delegate: OptionSelectorDelegate {
            objectName: "use" + modelData
            text: getNameFromIndex(index)
        }
        selectedIndex: [true, sims[0].connMan.powered, sims[1].connMan.powered]
            .lastIndexOf(true)
        onDelegateClicked: {
            sims.forEach(function (sim) {
                sim.connMan.powered = (model[index] === sim.path);
            });
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
        model: {
            var sim = getOnlineSim();
            if (sim) {
                return sim.radioSettings.modemTechnologies;
            } else {
                return [];
            }
        }
        delegate: OptionSelectorDelegate {
            text: {
                return {
                    'gsm': i18n.tr("2G only (saves battery)"),
                    'umts': i18n.tr("2G/3G (faster)"),
                    'lte': i18n.tr("2G/3G/4G (faster)")
                }[modelData]
            }
        }
        visible: use.selectedIndex !== 0
        onDelegateClicked: {
            var sim = getOnlineSim();
            if (sim) {
                sim.radioSettings.technologyPreference = model[index];
            }
        }
    }

    Binding {
        target: selector
        property: "enabled"
        value: getOnlineSim() &&
            (getOnlineSim().radioSettings.technologyPreference !== "")
        when: getOnlineSim()
    }

    Connections {
        target: sims[0].radioSettings
        onTechnologyPreferenceChanged: {
            if (sims[0].connMan.powered) {
                selector.selectedIndex = selector.model.indexOf(preference);
            }
        }
    }

    Connections {
        target: sims[1].radioSettings
        onTechnologyPreferenceChanged: {
            if (sims[1].connMan.powered) {
                selector.selectedIndex = selector.model.indexOf(preference);
            }
        }
    }

    ListItem.Standard {
        id: dataRoamingItem
        objectName: "dataRoamingSwitch"
        text: i18n.tr("Data roaming")
        enabled: use.selectedIndex !== 0
        control: Switch {
            id: dataRoamingControl
            onClicked: getOnlineSim().connMan.roamingAllowed = checked
        }
    }

    Binding {
        target: dataRoamingControl
        property: "checked"
        value: getOnlineSim() && getOnlineSim().connMan.roamingAllowed
        when: getOnlineSim()
    }
}
