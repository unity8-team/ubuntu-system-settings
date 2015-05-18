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

/* This is a temporary solution to the issue of Hotspots failing on mako. If
the device is mako, we hide the hotspot entry. Will be removed once lp:1434591
has been resolved. */
import Ubuntu.SystemSettings.Update 1.0

Column {

    objectName: "singleSim"

    property var sim

    /*  @sim a Sim.qml component containing libqofono bindings
        @prevOnlineModem path to modem that was online before this event */
    signal umtsModemChanged (var sim, string prevOnlineModem);

    ListItem.Standard {
        id: selector
        text: i18n.tr("Cellular data:")
        control: Switch {
            id: dataControl
            objectName: 'data'
            property bool serverChecked: sim.connMan.powered
            onServerCheckedChanged: checked = serverChecked
            Component.onCompleted: checked = serverChecked
            onTriggered: sim.connMan.powered = checked
        }
    }

    ListItem.Standard {
        id: dataRoamingItem
        text: i18n.tr("Data roaming")
        enabled: sim.connMan.powered
        showDivider: showAllUI
        control: Switch {
            id: dataRoamingControl
            objectName: "roaming"
            property bool serverChecked: sim.connMan.roamingAllowed
            onServerCheckedChanged: checked = serverChecked
            Component.onCompleted: checked = serverChecked
            onTriggered: sim.connMan.roamingAllowed = checked
        }
    }

    ListItem.SingleValue {
        text : i18n.tr("Hotspot disabled because Wi-Fi is off.")
        visible: showAllUI && !hotspotItem.visible &&
                 UpdateManager.deviceName !== "mako"
    }

    ListItem.SingleValue {
        id: hotspotItem
        objectName: "hotspotEntry"
        text: i18n.tr("Wi-Fi hotspot")
        progression: true
        onClicked: {
            pageStack.push(Qt.resolvedUrl("../Hotspot.qml"))
        }
        visible: showAllUI &&
                 (actionGroup.actionObject.valid ?
                     actionGroup.actionObject.state : false) &&
                 UpdateManager.deviceName !== "mako"
    }

    ListItem.Standard {
        text: i18n.tr("Data usage statistics")
        progression: true
        visible: showAllUI
    }

    ListItem.Divider {
        visible: radio.visible
    }

    RadioSingleSim {
        id: radio
        anchors { left: parent.left; right: parent.right }
        visible: radio.enabled
    }

    ListItem.Divider {}

    ListItem.SingleValue {
        text: i18n.tr("Carrier");
        id: chooseCarrier
        objectName: "carrierApnEntry"
        progression: enabled
        onClicked: pageStack.push(Qt.resolvedUrl("../PageCarrierAndApn.qml"), {
            sim: sim
        })
    }
}
