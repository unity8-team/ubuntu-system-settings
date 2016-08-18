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
import Ubuntu.Connectivity 1.0

Column {

    SortFilterModel {
        id: sortedModems
        model: Connectivity.modems
        sort.property: "Index"
        sort.order: Qt.AscendingOrder
    }

    ListItem.Standard {
        text: i18n.tr("Cellular data")
        control: Switch {
            id: dataSwitch
            objectName: "data"
            checked: Connectivity.mobileDataEnabled
            enabled: simSelector.currentSim !== null
            onTriggered: {
                Connectivity.mobileDataEnabled = checked
                /*
                 * We do this binding here to workaround bug:
                 * https://bugs.launchpad.net/ubuntu/+source/ubuntu-ui-toolkit/+bug/1494387
                 *
                 * The bug causes the checked binding to be overridden if plain onTriggered is used.
                 */
                checked = Qt.binding(function() {
                    return Connectivity.mobileDataEnabled
                })
            }
        }
    }

    ListItem.ItemSelector {
        id: simSelector
        showDivider: false
        expanded: true
        model: sortedModems
        selectedIndex: -1

        delegate: OptionSelectorDelegate {
            objectName: "use/ril_" + (model.Index - 1)
            text: {
                if (model.Sim) {
                    return  sims[model.Index - 1].name + " (" +
                            (model.Sim.PrimaryPhoneNumber !== "" ?
                                model.Sim.PrimaryPhoneNumber : model.Sim.Imsi) + ")"
                }
                else {
                    return i18n.tr("No SIM detected")
                }
            }
            subText: {
                if (model.Sim) {
                    return ""
                }
                else {
                    return i18n.tr("Insert a SIM, then restart the device.")
                }
            }
            enabled: model.Sim !== null
        }

        property var currentSim : null
        onSelectedIndexChanged: {
            if (selectedIndex === -1) {
                currentSim = null
            } else {
                currentSim = model.get(selectedIndex).Sim
            }
        }
        onCurrentSimChanged: {
            if (currentSim !== null) {
                roaming.checked = Qt.binding(function() {
                    return currentSim.DataRoamingEnabled
                })
            }
        }

        function setSelectedIndex() {
            if (Connectivity.simForMobileData === null) {
                simSelector.selectedIndex = -1
                return
            }

            for (var i = 0; i < sortedModems.count; i++) {
                if (sortedModems.get(i).Sim === Connectivity.simForMobileData) {
                    simSelector.selectedIndex = i
                    return
                }
            }
            simSelector.selectedIndex = -1
        }
        Connections {
            target: Connectivity
            onSimForMobileDataUpdated: {
                simSelector.setSelectedIndex()
            }
        }
        Component.onCompleted: {
            setSelectedIndex()
        }

        onTriggered:  {
            // @bug: https://bugs.launchpad.net/ubuntu/+source/ubuntu-ui-toolkit/+bug/1577351
            // Connectivity.simForMobileData = currentSim
            /*
             * There is a bug in onTriggered that causes it to be fired *before* selectedIndex has been updated
             * and thus there is no way of knowing what index was actually selected by the user.
             *
             * This is worked around below by using onDelegateClicked which gives us the missing index.
             */
        }
        onDelegateClicked: {
            var sim = sortedModems.get(index).Sim
            if (sim === null) {
                return
            }
            Connectivity.simForMobileData = sim
        }
    }

    ListItem.Standard {
        text: i18n.tr("Data roaming")
        control: Switch {
            id: roaming
            objectName: "roaming"
            enabled: simSelector.currentSim !== null && dataSwitch.checked
            checked: simSelector.currentSim ? simSelector.currentSim.DataRoamingEnabled : false
            function trigger() {
                simSelector.currentSim.DataRoamingEnabled = !checked
            }
        }
    }
}
