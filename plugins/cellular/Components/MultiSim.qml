/*
 * Copyright (C) 2014-2016 Canonical Ltd
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
import GSettings 1.0
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.Cellular 1.0
import Ubuntu.Components.ListItems 1.3 as ListItems

Column {
    id: multiSim
    objectName: "multiSim"
    property var sims
    property var poweredSim: {
        var s = null;
        sims.forEach(function (sim) {
            if (sim.connMan.powered === true) {
                s = sim;
            }
        });
        return s;
    }
    property var modems
    property var settings: phoneSettings
    property string prevOnlineModem: ""

    /*  @sim a Sim.qml component containing libqofono bindings
        @prevOnlineModem path to modem that was online before modem reset */
    signal umtsModemChanged (var sim, string prevOnlineModem);

    DataMultiSim {
        anchors { left: parent.left; right: parent.right }
    }

    SettingsListItems.StandardProgression {
        id: dataUsage
        text: i18n.tr("Data usage statistics")
        visible: showAllUI
    }

    SettingsListItems.SingleValueProgression {
        text: i18n.tr("Carriers")
        id: chooseCarrier
        objectName: "carrierApnEntry"
        progressionVisible: enabled
        showDivider: false
        onClicked: {
            pageStack.addPageToNextColumn(multiSim,
                Qt.resolvedUrl("../PageCarriersAndApns.qml"), { sims: sims });
        }
    }

    SimEditor {
        anchors { left: parent.left; right: parent.right }
    }

    ListItems.Divider {}

    DefaultSim {
        anchors { left: parent.left; right: parent.right }
    }

    ListItems.Divider {}

    SettingsItemTitle { text: i18n.tr("Connection type:") }

    Repeater {
        model: sims

        ListItems.ItemSelector {
            id: radio
            property var sim: modelData

            expanded: true
            text: sim.title
            model: sim.radioSettings.availableTechnologies
            delegate: OptionSelectorDelegate {
                objectName: sim.path + "_radio_" + modelData
                text: sim.techToString(modelData)
            }
            enabled: sim.radioSettings.technologyPreference !== ""
            selectedIndex: sim.radioSettings.technologyPreference !== "" ?
                model.indexOf(sim.radioSettings.technologyPreference) : -1

            onDelegateClicked: {
                if (model[index] === 'umts_enable') {
                    sim.radioSettings.technologyPreference = 'umts';
                    umtsModemChanged(sim, poweredSim ? poweredSim.path : "");
                    sim.mtkSettings.has3G = true;
                } else {
                    sim.radioSettings.technologyPreference = model[index];
                }
            }

            Connections {
                target: sim.radioSettings
                onTechnologyPreferenceChanged: radio.selectedIndex =
                    sim.radioSettings.availableTechnologies.indexOf(preference)

                onAvailableTechnologiesChanged: {
                    if ((technologies.indexOf('umts') === -1)
                         && (sim.mtkSettings.has3G === false)) {
                        radio.model = sim.addUmtsEnableToModel(technologies);
                    } else {
                        radio.model = technologies;
                    }
                    radio.selectedIndex = sim.radioSettings.technologyPreference !== "" ?
                        model.indexOf(sim.radioSettings.technologyPreference) : -1
                }
                ignoreUnknownSignals: true
            }

            Component.onCompleted: {
                if ((sim.radioSettings.availableTechnologies.indexOf('umts') === -1)
                     && (sim.mtkSettings.has3G === false)) {
                    radio.model = sim.addUmtsEnableToModel(sim.radioSettings.availableTechnologies);
                } else {
                    radio.model = sim.radioSettings.availableTechnologies;
                }
            }
        }
    }


    UbuntuCellularPanel {
        id: phoneSettings
        Component.onCompleted: {
            // set default names
            var simNames = phoneSettings.simNames;
            var m0 = modems[0];
            var m1 = modems[1];
            if (!simNames[m0]) {
                simNames[m0] = "SIM 1";
            }
            if (!simNames[m1]) {
                simNames[m1] = "SIM 2";
            }
            phoneSettings.simNames = simNames;
        }
    }

    Binding {
        target: sims[0]
        property: "name"
        value: phoneSettings.simNames[modems[0]]
    }

    Binding {
        target: sims[1]
        property: "name"
        value: phoneSettings.simNames[modems[1]]
    }
}
