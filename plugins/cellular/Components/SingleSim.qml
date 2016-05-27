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
import Ubuntu.Connectivity 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

Column {

    objectName: "singleSim"
    id: singlesim

    property var sim

    /*  @sim a Sim.qml component containing libqofono bindings
        @prevOnlineModem path to modem that was online before this event */
    signal umtsModemChanged (var sim, string prevOnlineModem);

    property var currentSim
    Component.onCompleted: {
        currentSim = sortedModems.get(0).Sim
        Connectivity.simForMobileData = currentSim
    }
    SortFilterModel {
        id: sortedModems
        model: Connectivity.modems
        sort.property: "Index"
        sort.order: Qt.AscendingOrder
    }

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: units.gu(2)
        anchors.margins: units.gu(2)

        RowLayout {
            Layout.topMargin: units.gu(2)
            Label {
                text: i18n.tr("Cellular data")
                Layout.fillWidth: true
            }
            Switch {
                id: dataSwitch
                objectName: "data"
                checked: Connectivity.mobileDataEnabled
                onTriggered: {
                    Connectivity.mobileDataEnabled = checked
                    checked = Qt.binding(function() {
                        return Connectivity.mobileDataEnabled
                    })
                }
            }
        }
        ColumnLayout {
            Layout.bottomMargin: units.gu(2)
            spacing: units.gu(2)

            RowLayout {
                Label {
                    text: i18n.tr("Data roaming")
                    Layout.fillWidth: true
                }
                Switch {
                    id: dataRoamingSwitch
                    objectName: "roaming"
                    enabled: singlesim.currentSim !== null && dataSwitch.checked
                    checked: singlesim.currentSim.DataRoamingEnabled
                    function trigger() {
                        singlesim.currentSim.DataRoamingEnabled = !checked
                    }
                }
            }
        }
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
