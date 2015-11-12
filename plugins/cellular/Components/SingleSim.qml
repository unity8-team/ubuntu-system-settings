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
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.SystemSettings.Cellular 1.0
import Ubuntu.Components 1.3

Column {

    objectName: "singleSim"

    property var sim

    /*  @sim a Sim.qml component containing libqofono bindings
        @prevOnlineModem path to modem that was online before this event */
    signal umtsModemChanged (var sim, string prevOnlineModem);

    SettingsListItems.Standard {
        id: selector
        text: i18n.tr("Cellular data:")

        Switch {
            id: dataControl
            objectName: 'data'
            property bool serverChecked: sim.connMan.powered
            onServerCheckedChanged: checked = serverChecked
            Component.onCompleted: checked = serverChecked
            onTriggered: sim.connMan.powered = checked
        }
    }

    SettingsListItems.Standard {
        id: dataRoamingItem
        text: i18n.tr("Data roaming")
        enabled: sim.connMan.powered

        Switch {
            id: dataRoamingControl
            objectName: "roaming"
            property bool serverChecked: sim.connMan.roamingAllowed
            onServerCheckedChanged: checked = serverChecked
            Component.onCompleted: checked = serverChecked
            onTriggered: sim.connMan.roamingAllowed = checked
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
        onClicked: pageStack.push(Qt.resolvedUrl("../PageCarrierAndApn.qml"), {
            sim: sim
        })
    }

    SettingsListItems.SingleValueProgression {
        text: i18n.tr("Connection type")
        value: sim.getTechString()
        onClicked: pageStack.push(Qt.resolvedUrl("../ConnectionType.qml"), {
            sim: sim
        })
    }
}
