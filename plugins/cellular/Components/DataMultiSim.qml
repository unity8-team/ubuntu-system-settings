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

    function getNameFromIndex (index) {
        if (index === 0) {
            return i18n.tr("Off");
        } else if (index > 0) {
            return sims[index - 1].title;
        }
    }

    height: childrenRect.height

    ListItem.ItemSelector {
        id: use
        objectName: "data"
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
            // power all sims on or off
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

    ListItem.Standard {
        id: dataRoamingItem
        text: i18n.tr("Data roaming")
        enabled: use.selectedIndex !== 0
        control: Switch {
            id: dataRoamingControl
            objectName: "roaming"
            onClicked: poweredSim.connMan.roamingAllowed = checked
        }
        showDivider: false
    }

    Binding {
        target: dataRoamingControl
        property: "checked"
        value: poweredSim && poweredSim.connMan.roamingAllowed
    }
}
