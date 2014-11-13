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

    objectName: "singleSim"

    property var sim

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
        showDivider: false
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
        visible: showAllUI && !hotspotItem.visible
    }

    ListItem.SingleValue {
        id: hotspotItem
        text: i18n.tr("Wi-Fi hotspot")
        progression: true
        onClicked: {
            pageStack.push(Qt.resolvedUrl("Hotspot.qml"))
        }
        visible: showAllUI && (actionGroup.actionObject.valid ? actionGroup.actionObject.state : false)
    }

    ListItem.Standard {
        text: i18n.tr("Data usage statistics")
        progression: true
        visible: showAllUI
    }

    ListItem.Divider {
        visible: radio.selector.model.length
    }

    RadioSingleSim {
        id: radio
        anchors { left: parent.left; right: parent.right }
        visible: radio.selector.model.length
    }

    ListItem.Divider {}

    ListItem.SingleValue {
        text: i18n.tr("Carrier");
        id: chooseCarrier
        objectName: "carrier"
        progression: enabled
        value: sim.netReg.name || i18n.tr("N/A")
        enabled: sim.netReg.status !== ""
        onClicked: {
            pageStack.push(Qt.resolvedUrl("../PageChooseCarrier.qml"), {
                sim: sim,
                title: i18n.tr("Carrier")
            })
        }
    }

    ListItem.Standard {
        text: i18n.tr("APN")
        progression: true
        visible: showAllUI
    }
}
