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
import SystemSettings 1.0
import Ubuntu.Connectivity 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3

Column {

    objectName: "singleSim"
    id: singlesim

    property var sim

    /*  @sim a Sim.qml component containing libqofono bindings
        @prevOnlineModem path to modem that was online before this event */
    signal umtsModemChanged (var sim, string prevOnlineModem);

    property var currentSim
    Component.onCompleted: {
        if (sortedModems.count === 1)
        {
            currentSim = sortedModems.get(0).Sim
            Connectivity.simForMobileData = currentSim
        }
        if (sortedModems.count === 2 &&
                (sortedModems.get(0).Sim === null ||
                 sortedModems.get(1).Sim === null))
        {
            // Dual-SIM phone with only one sim present
            var sim = sortedModems.get(0).Sim
            if (sim === null)
            {
                sim = sortedModems.get(1).Sim
            }
            if (sim !== null)
            {
                currentSim = sim
                Connectivity.simForMobileData = sim
            }
        }
    }
    SortFilterModel {
        id: sortedModems
        model: Connectivity.modems
        sort.property: "Index"
        sort.order: Qt.AscendingOrder
    }
    SettingsListItems.Standard {
        id: selector
        text: i18n.tr("Cellular data:")
        Switch {
            id: dataSwitch
            objectName: "data"
            checked: Connectivity.mobileDataEnabled
            enabled: singlesim.currentSim !== null
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

    SettingsListItems.Standard {
        id: dataRoamingItem
        text: i18n.tr("Data roaming")
        Switch {
            id: roaming
            objectName: "roaming"
            enabled: singlesim.currentSim !== null && dataSwitch.checked
            checked: singlesim.currentSim.DataRoamingEnabled
            function trigger() {
                singlesim.currentSim.DataRoamingEnabled = !checked
            }
        }
    }

    SettingsListItems.StandardProgression{
        text: i18n.tr("Data usage statistics")
        visible: showAllUI
    }

    SettingsListItems.SingleValueProgression {
        text: i18n.tr("Carrier & APN");
        id: chooseCarrier
        objectName: "carrierApnEntry"
        value: sim.netReg.name || ""
        onClicked: pageStack.addPageToNextColumn(singlesim,
            Qt.resolvedUrl("../PageCarrierAndApn.qml"), { sim: sim })
    }

    RadioSingleSim {
        id: radio
        anchors { left: parent.left; right: parent.right }
        visible: radio.enabled
    }
}
