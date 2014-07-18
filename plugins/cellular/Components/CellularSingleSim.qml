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
    height: childrenRect.height
    //width: parent.width

    property bool dataEnabled
    property bool roamingAllowed

    property alias sim: root.sims[0]
    property alias radioSettings: root.sims[0].radioSettings
    property alias connMan: root.sims[0].connMan

    property var selector: selector

    ListItem.ItemSelector {
        id: selector
        objectName: "technologyPreferenceSelector"
        expanded: true
        model: [i18n.tr("Off"), i18n.tr("2G only (saves battery)"), i18n.tr("2G/3G/4G (faster)")]
        onDelegateClicked: DataHelpers.techSelectorClicked(index)
    }

    Connections {
        target: connMan
        onPoweredChanged: DataHelpers.poweredChanged(powered)
    }

    Connections {
        target: radioSettings
        onTechnologyPreferenceChanged: DataHelpers.preferenceChanged(preference)
    }

    Binding {
        id: powerBinding
        property: "powered"
        value: selector.selectedIndex !== 0
    }

    Binding {
        id: dataEnabledBinding
        property: "powered"
        value: dataEnabled
    }

    Binding {
        id: roamingBinding
        property: "roamingAllowed"
        value: roamingAllowed
    }


    Component.onCompleted: {
        powerBinding.target = connMan
        dataEnabled.target = connMan
        roamingBinding.target = connMan

        // technologyPreference "" is not valid, assume sim locked or data unavailable
        selector.enabled = radioSettings.technologyPreference !== ""
        selector.selectedIndex = function () {
            var pref = radioSettings.technologyPreference;
            // make nothing selected if the string from OfonoRadioSettings is empty
            if (pref === "") {
                console.warn("Disabling TechnologyPreference item selector due to empty TechnologyPreference");
                return -1;
            } else {
                // normalizeKey turns "lte" and "umts" into "any"
                return DataHelpers.keyToIndex(DataHelpers.normalizeKey(pref));
            }
        }
    }
}
