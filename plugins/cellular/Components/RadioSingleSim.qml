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
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Column {
    id: radioSingleSim
    height: childrenRect.height

    property bool enabled: sim.radioSettings.technologyPreference !== ""

    SettingsItemTitle { text: i18n.tr("Connection type:") }

    ListItem.ItemSelector {
        id: selector

        function techToString (tech) {
            var strings = {
                'gsm': i18n.tr("2G only (saves battery)"),
                'umts': i18n.tr("2G/3G (faster)"),
                'lte': i18n.tr("2G/3G/4G (faster)")
            };
            strings['umts_enable'] = strings['umts'] + '*';
            return strings[tech];
        }

        // adds umts_enable to an copy of model
        function addUmtsEnableToModel (model) {
            var newModel = model.slice(0);
            newModel.push('umts_enable');
            return newModel;
        }

        showDivider: false
        expanded: true

        // an empty string is not a valid preference, which means
        // we disregard the interace and disable the selector
        enabled: parent.enabled
        model: sim.radioSettings.modemTechnologies
        delegate: OptionSelectorDelegate {
            objectName: sim.path + "_radio_" + modelData
            text: selector.techToString(modelData)
            showDivider: false
        }
        selectedIndex:
            sim.radioSettings.technologyPreference !== "" ?
                    model.indexOf(sim.radioSettings.technologyPreference) : -1

        onDelegateClicked: {
            if (model[index] === 'umts_enable') {
                sim.radioSettings.technologyPreference = 'umts';
                radioSingleSim.parent.umtsModemChanged(sim);
                sim.mtkSettings.has3G = true;
            } else {
                sim.radioSettings.technologyPreference = model[index];
            }
        }

        Connections {
            target: sim.radioSettings
            onTechnologyPreferenceChanged: selector.selectedIndex =
                sim.radioSettings.modemTechnologies.indexOf(preference)
            onModemTechnologiesChanged: {
                if ((technologies.indexOf('umts') === -1)
                     && (sim.mtkSettings.has3G === false)) {
                    selector.model = selector.addUmtsEnableToModel(technologies);
                } else {
                    selector.model = technologies;
                }
                selector.selectedIndex = sim.radioSettings.technologyPreference !== "" ?
                    selector.model.indexOf(sim.radioSettings.technologyPreference) : -1
            }
            ignoreUnknownSignals: true
        }

        Component.onCompleted: {
            if ((sim.radioSettings.modemTechnologies.indexOf('umts') === -1)
                 && (sim.mtkSettings.has3G === false)) {
                selector.model = selector.addUmtsEnableToModel(sim.radioSettings.modemTechnologies);
            } else {
                selector.model = sim.radioSettings.modemTechnologies;
            }
        }
    }
}
