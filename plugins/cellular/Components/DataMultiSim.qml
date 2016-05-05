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
import QtQuick.Layouts 1.2

import SystemSettings 1.0

import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

import Ubuntu.Connectivity 1.1

ColumnLayout {
    anchors.margins: units.gu(2)

    SortFilterModel {
        id: sortedModems
        model: Connectivity.modems
        sort.property: "Index"
        sort.order: Qt.AscendingOrder
    }

    ColumnLayout {
        spacing: units.gu(2)

        RowLayout {
            Layout.topMargin: units.gu(2)
            Label {
                text: i18n.tr("Cellular data")
                Layout.fillWidth: true
            }
            Switch {
                id: dataSwitch
                checked: Connectivity.mobileDataEnabled
                enabled: simSelector.currentSim !== null
                function trigger() {
                    Connectivity.mobileDataEnabled = !checked
                }
            }
        }
        ColumnLayout {
            Layout.leftMargin: units.gu(2)
            spacing: units.gu(2)
            OptionSelector {
                id: simSelector
                expanded: currentSim === null
                model: sortedModems
                selectedIndex: -1

                delegate: OptionSelectorDelegate {
                    text: {
                        if (model.Sim) {
                            circled(model.Index) + " " + model.Sim.PrimaryPhoneNumber
                        }
                        else {
                            return circled(model.Index) + " " + i18n.tr("No SIM detected")
                        }
                    }
                    subText: {
                        if (model.Sim) {
                            return ""
                        }
                        else {
                            return i18n.tr("Insert a SIM, then restart the phone.")
                        }
                    }
                    //enabled: model.Sim !== null // https://bugs.launchpad.net/ubuntu/+source/ubuntu-ui-toolkit/+bug/1577359

                    function circled(index) {
                        if (index === 1) {
                            return "①"
                        } else if (index === 2) {
                            return "②"
                        }

                        return " "
                    }
                }

                property var currentSim : null
                onSelectedIndexChanged: {
                    if (selectedIndex == -1) {
                        currentSim = null
                    } else {
                        currentSim = model.get(selectedIndex).Sim
                    }
                }
                onCurrentSimChanged: {
                    if (currentSim != null) {
                        dataRoamingSwitch.checked = Qt.binding(function() {
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
                }
                onDelegateClicked: {
                    var sim = sortedModems.get(index).Sim
                    if (sim === null) {
                        return
                    }
                    Connectivity.simForMobileData = sim
                }
            }
        }
        RowLayout {
            Layout.bottomMargin: units.gu(2)
            Label {
                text: i18n.tr("Data roaming")
                Layout.fillWidth: true
            }
            Switch {
                id: dataRoamingSwitch
                enabled: simSelector.currentSim !== null && dataSwitch.checked
                checked: simSelector.currentSim ? simSelector.currentSim.DataRoamingEnabled : false
                function trigger() {
                    simSelector.currentSim.DataRoamingEnabled = !checked
                }
            }
        }
    }
}
