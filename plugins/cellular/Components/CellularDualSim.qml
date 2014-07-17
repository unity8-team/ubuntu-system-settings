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
//import "dualdata-helpers.js" as DualDataHelpers

Column {

    height: childrenRect.height
    width: parent.width

    function getRoamingAllowed () {
        var poweredModem = getPoweredModem();
        if (poweredModem) {
            console.warn('getRoamingAllowed returns', poweredModem.connMan.roamingAllowed);
            return poweredModem.connMan.roamingAllowed;
        } else {
            console.warn('getRoamingAllowed returns false');
            return false;
        }
    }

    function getPoweredModem () {
        if (sim1.connMan.powered) {
            console.warn('getPoweredModem: sim1 is powered');
            return sim1;
        } else if (sim2.connMan.powered) {
            console.warn('getPoweredModem: sim2 is powered');
            return sim2;
        } else {
            console.warn('getPoweredModem: no sim is powered');
            return null;
        }
    }

    function getRadioSettings () {
        var poweredModem = getPoweredModem();
        if (poweredModem) {
            console.warn('getRadioSettings returns actual radioSettings', poweredModem.radioSettings);
            return poweredModem.radioSettings;
        } else {
            console.warn('getRadioSettings returns null');
            return null;
        }
    }

    property var sim1: sims[0]
    property var sim2: sims[1]
    property var selector: selector

    property bool dataEnabled: getPoweredModem()
    property bool roamingAllowed: getRoamingAllowed()
    property var radioSettings: getRadioSettings()

    ListItem.ItemSelector {
        id: use
        objectName: "use"
        text: i18n.tr("Cellular data:")
        expanded: true
        selectedIndex: {
            if (sim1.connMan.powered) {
                return 1;
            } else if (sim2.connMan.powered) {
                return 2;
            } else {
                // both off
                // TODO: or both on
                return 0
            }
        }
        model: [i18n.tr("Off"), sim1.title, sim2.title]
    }

    ListItem.ItemSelector {
        id: selector
        objectName: "technologyPreferenceSelector"
        expanded: true
        model: [i18n.tr("2G only (saves battery)"), i18n.tr("2G/3G/4G (faster)")]

        // // technologyPreference "" is not valid, assume sim locked or data unavailable
        // enabled: radioSettings && radioSettings.technologyPreference !== ""
        // selectedIndex: {
        //     var pref = radioSettings ? radioSettings.technologyPreference : "";
        //     // make nothing selected if the string from OfonoRadioSettings is empty
        //     if (pref === "") {
        //         console.warn("Disabling TechnologyPreference item selector due to empty TechnologyPreference");
        //         return -1;
        //     } else {
        //         // normalizeKey turns "lte" and "umts" into "any"
        //         return DataHelpers.keyToIndex(DataHelpers.normalizeKey(pref));
        //     }
        // }
        // onDelegateClicked: DataHelpers.delegateClicked(index)
    }
}
