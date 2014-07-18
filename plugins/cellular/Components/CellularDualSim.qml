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
    property var sim2
    property var selector: selector

    function getSelectedSim () {
        var i = use.selectedIndex;
        if (i === 1) {
            return sim1;
        } else if (i === 2) {
            return sim2;
        } else {
            return null;
        }
    }

    ListItem.ItemSelector {
        id: use
        objectName: "use"
        text: i18n.tr("Cellular data:")
        expanded: true
        model: [i18n.tr("Off"), sim1.title, sim2.title]
        selectedIndex: [true, sim1.connMan.powered, sim2.connMan.powered].lastIndexOf(true);
        onDelegateClicked: DataHelpers.simSelectorClicked(index)
    }

    ListItem.ItemSelector {
        id: selector
        objectName: "technologyPreferenceSelector"
        expanded: true
        model: [i18n.tr("2G only (saves battery)"), i18n.tr("2G/3G/4G (faster)")]

        // technologyPreference "" is not valid, assume sim locked or data unavailable
        enabled: getSelectedSim() && (getSelectedSim().radioSettings.technologyPreference !== "")
        selectedIndex: {
            var pref = getSelectedSim() ? getSelectedSim().radioSettings.technologyPreference : "";
            console.warn('selectedIndex pref', pref, getSelectedSim() ? getSelectedSim().radioSettings.technologyPreference : "");
            // make nothing selected if the string from OfonoRadioSettings is empty
            if (pref === "") {
                console.warn("Disabling TechnologyPreference item selector due to empty TechnologyPreference");
                return -1;
            } else {
                // normalizeKey turns "lte" and "umts" into "any"
                return DataHelpers.dualKeyToIndex(DataHelpers.normalizeKey(pref));
            }
        }
        onDelegateClicked: DataHelpers.dualTechSelectorClicked(index)
    }

    ListItem.Standard {
        id: dataRoamingItem
        objectName: "dataRoamingSwitch"
        text: i18n.tr("Data roaming")
        enabled: getSelectedSim() ? getSelectedSim().connMan.powered : false
        control: Switch {
            id: dataRoamingControl
            checked: getSelectedSim() ? getSelectedSim().connMan.powered : false
            onClicked: getSelectedSim().connMan.roamingAllowed = checked
        }
    }


    Component.onCompleted: {
        // now what
        if (sim1.connMan.powered && sim2.connMan.powered) {
            sim2.connMan.powered = false;
        }
    }
}
