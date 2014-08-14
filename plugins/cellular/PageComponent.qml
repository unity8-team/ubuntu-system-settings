/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import GSettings 1.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import MeeGo.QOfono 0.2
import QMenuModel 0.1
import "Components"

ItemPage {
    id: root
    title: i18n.tr("Cellular")
    objectName: "cellularPage"

    // pointers to sim 1 and 2, lazy loaded
    property alias sim1: simOneLoader.item
    property alias sim2: simTwoLoader.item
    property var modemsSorted: manager.modems.slice(0).sort()

    states: [
        State {
            name: "singleSim"
            StateChangeScript {
                name: "loadSim"
                script: {
                    var p = modemsSorted[0];
                    simOneLoader.setSource("Components/Sim.qml", {
                        path: p
                    });
                }
            }
        },
        State {
            name: "dualSim"
            extend: "singleSim"
            StateChangeScript {
                name: "loadSecondSim"
                script: {
                    var p = modemsSorted[1];
                    simTwoLoader.setSource("Components/Sim.qml", {
                        path: p
                    });
                    defaultSimLoader.source = "Components/DefaultSim.qml";
                }
            }
        }
    ]

    QDBusActionGroup {
        id: actionGroup
        busType: 1
        busName: "com.canonical.indicator.network"
        objectPath: "/com/canonical/indicator/network"

        property variant actionObject: action("wifi.enable")

        Component.onCompleted: {
            start()
        }
    }

    OfonoManager {
        id: manager
        Component.onCompleted: {
            if (modems.length === 1) {
                root.state = "singleSim";
            } else if (modems.length === 2) {
                root.state = "dualSim";
            }
        }
    }

    Loader {
        id: simOneLoader
        onLoaded: {
            if (parent.state === "singleSim") {
                cellData.setSource("Components/CellularSingleSim.qml", {
                    sim1: sim1
                });
            }
        }
    }

    Loader {
        id: simTwoLoader
        onLoaded: {
            // unload any single sim setup
            cellData.source = "";
            cellData.setSource("Components/CellularDualSim.qml", {
                sim1: sim1,
                sim2: sim2
            });
            simEditorLoader.source = "Components/SimEditor.qml";
        }
    }

    OfonoSimManager {
        id: sim
        modemPath: manager.modems[0]
    }

    OfonoNetworkRegistration {
        id: netReg
        modemPath: manager.modems[0]
        onStatusChanged: {
            console.warn ("onStatusChanged: " + netReg.status);
        }
    }

    OfonoConnMan {
        id: connMan
        modemPath: manager.modems[0]
        // FIXME powered: techPrefSelector.selectedIndex !== 0
        // FIXME onPoweredChanged: RSHelpers.poweredChanged(powered);
    }

    OfonoModem {
        id: modem
        modemPath: manager.modems[0]
    }

    Flickable {
        anchors.fill: parent
        //contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors { left: parent.left; right: parent.right }

            ListModel {
                id: techPrefModel
                ListElement { name: "Off"; key: "off" }
                ListElement { name: "2G only (saves battery)"; key: "gsm"; }
                ListElement { name: "2G/3G/4G (faster)"; key: "any"; }
            }

            Component {
                id: techPrefDelegate
                OptionSelectorDelegate { text: i18n.tr(name); }
            }

            Loader {
                id: cellData
                anchors { left: parent.left; right: parent.right }
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

            ListItem.SingleValue {
                text: i18n.tr("Carrier", "Carriers", manager.modems.length);
                id: chooseCarrier
                objectName: "chooseCarrier"
                value: netReg.name ? netReg.name : i18n.tr("N/A")
                progression: true
                onClicked: {
                    if (root.state === 'singleSim') {
                        pageStack.push(Qt.resolvedUrl("PageChooseCarrier.qml"), {
                            netReg: sim1.netReg,
                            connMan: connMan,
                            title: i18n.tr("Carrier")
                        })
                    } else if (root.state === 'dualSim') {
                        pageStack.push(Qt.resolvedUrl("PageChooseCarriers.qml"), {
                            sim1: sim1,
                            sim2: sim2
                        });
                    }
                }
            }

            Binding {
                target: chooseCarrier
                property: "value"
                value: sim1.netReg.name || i18n.tr("N/A")
                when: (simOneLoader.status === Loader.Ready) && root.state === "singleSim"
            }

            ListItem.Standard {
                text: i18n.tr("APN")
                progression: true
                visible: showAllUI
            }

            Loader {
                id: simEditorLoader
                anchors { left: parent.left; right: parent.right }
            }

            ListItem.Divider {}

            Loader {
                id: defaultSimLoader
                anchors.left: parent.left
                anchors.right: parent.right
            }
        }
    }

    GSettings {
        id: phoneSettings
        schema.id: "com.ubuntu.phone"
        Component.onCompleted: {
            // set default names
            var simNames = phoneSettings.simNames;
            var m0 = modemsSorted[0];
            var m1 = modemsSorted[1];
            if (!simNames[m0]) {
                simNames[m0] = "SIM 1";
            }
            if (!simNames[m1]) {
                simNames[m1] = "SIM 2";
            }
            phoneSettings.simNames = simNames;
        }
    }

    Binding {
        target: sim1
        property: "name"
        value: phoneSettings.simNames[modemsSorted[0]]
    }

    Binding {
        target: sim2
        property: "name"
        value: phoneSettings.simNames[modemsSorted[1]]
    }
}
