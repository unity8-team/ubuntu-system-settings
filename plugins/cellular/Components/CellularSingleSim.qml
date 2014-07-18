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
        expanded: true
        enabled: sim1.radioSettings.technologyPreference !== ""
        model: [i18n.tr("Off"), i18n.tr("2G only (saves battery)"), i18n.tr("2G/3G/4G (faster)")]
        selectedIndex: {
            var pref = sim1.radioSettings.technologyPreference;
            // make nothing selected if the string from OfonoRadioSettings is empty
            if (pref === "") {
                console.warn("Disabling TechnologyPreference item selector due to empty TechnologyPreference");
                return -1;
            } else {
                return DataHelpers.singleKeyToIndex(pref);
            }
        }
        onDelegateClicked: {
            console.warn('selector clicked:', index);
            var k;
            // if the user selects a TechnologyPreference, update RadioSettings
            if (index > 0) {
                k = DataHelpers.singleIndexToKey(index);
                sim1.radioSettings.technologyPreference = k;
                console.warn('selector clicked will set TechnologyPreference to', k);
            }
        }
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
            var rdoKey = sim1.radioSettings.technologyPreference;
            console.log('poweredChanged rdoKey', rdoKey, 'powered', powered);
            if (powered) {
                if (rdoKey === '') {
                    console.warn('Modem came online but TechnologyPreference is empty');
                    return;
                } else {
                    console.warn('Modem came online, TechnologyPreference', rdoKey, 'new selectedIndex', DataHelpers.singleKeyToIndex(rdoKey));
                    selector.selectedIndex = DataHelpers.singleKeyToIndex(rdoKey)
                }
            } else {
                console.warn('Modem went offline');
                selector.selectedIndex = 0;
            }
        }
    }

    Connections {
        target: sim1.radioSettings
        onTechnologyPreferenceChanged: {
            var i = selector.selectedIndex;
            var rdoKey = sim1.radioSettings.technologyPreference;
            var normalizedKey;

            // if preference changes, but the user has chosen one already make sure the user's setting is respected
            if (i > 0) {
                console.warn('Overriding RadioSettings TechnologyPreference signal', preference, 'with user selection', DataHelpers.singleIndexToKey(i));
                sim1.radioSettings.technologyPreference = DataHelpers.singleIndexToKey(i);
                return;
            }

            normalizedKey = DataHelpers.normalizeKey(rdoKey);
            // if the pref changes and the modem is on normlize and update the UI
            if (connMan.powered) {
                selector.selectedIndex = DataHelpers.singleKeyToIndex(normalizedKey);
            } else {
                // if the modem is off, just normalize
                sim1.radioSettings.technologyPreference = normalizedKey;
            }
            console.warn('Modem', connMan.powered ? 'online' : 'offline', 'TechnologyPreference', rdoKey);
        }
    }

    Binding {
        target: sim1.connMan
        property: "powered"
        value: selector.selectedIndex !== 0
    }
}
