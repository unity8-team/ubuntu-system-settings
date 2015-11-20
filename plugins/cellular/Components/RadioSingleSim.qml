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
import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

Column {
    id: radioSingleSim
    height: childrenRect.height

    property bool enabled: sim.radioSettings.technologyPreference !== ""

    SettingsItemTitle { text: i18n.tr("Connection type:") }

    ListItem.ItemSelector {
        id: selector

        showDivider: false
        expanded: true

        // an empty string is not a valid preference, which means
        // we disregard the interace and disable the selector
        enabled: parent.enabled
        model: sim.radioSettings.availableTechnologies
        delegate: OptionSelectorDelegate {
            objectName: sim.path + "_radio_" + modelData
            text: sim.techToString(modelData)
            showDivider: false
        }
        selectedIndex:
            sim.radioSettings.technologyPreference !== "" ?
                    model.indexOf(sim.radioSettings.technologyPreference) : -1

        onDelegateClicked: {
            if (model[index] === 'umts_enable') {
                sim.radioSettings.technologyPreference = 'umts';
                radioSingleSim.parent.umtsModemChanged(sim, sim.connMan.powered ? sim.path : "");
                sim.mtkSettings.has3G = true;
            } else {
                sim.radioSettings.technologyPreference = model[index];
            }
        }

        Connections {
            target: sim.radioSettings

            onTechnologyPreferenceChanged: selector.selectedIndex =
                sim.radioSettings.availableTechnologies.indexOf(preference)

            onAvailableTechnologiesChanged: {
                if ((technologies.indexOf('umts') === -1)
                     && (sim.mtkSettings.has3G === false)) {
                    selector.model = sim.addUmtsEnableToModel(technologies);
                } else {
                    selector.model = technologies;
                }
                selector.selectedIndex = sim.radioSettings.technologyPreference !== "" ?
                    selector.model.indexOf(sim.radioSettings.technologyPreference) : -1
            }
            ignoreUnknownSignals: true
        }

        Component.onCompleted: {
            if ((sim.radioSettings.availableTechnologies.indexOf('umts') === -1)
                 && (sim.mtkSettings.has3G === false)) {
                selector.model = sim.addUmtsEnableToModel(
                    sim.radioSettings.availableTechnologies);
            } else {
                selector.model = sim.radioSettings.availableTechnologies;
            }
        }
    }
}
