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

    states: [
        State {
            name: "singleSim"
            StateChangeScript {
                name: "loadSim"
                script: simOneLoader.setSource("Components/Sim.qml", {
                    path: manager.modems[0]
                })
            }
        },
        State {
            name: "dualSim"
            StateChangeScript {
                name: "loadSecondSim"
                script: {
                    // the ordering is completely depending on manager
                    // TODO: proper sim names (from gsettings?)
                    simOneLoader.setSource("Components/Sim.qml", {
                        path: manager.modems[0]
                    });
                    simTwoLoader.setSource("Components/Sim.qml", {
                        path: manager.modems[1]
                    });
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
            console.warn('Manager complete with', modems.length, 'sims.');
            for (var i=0, j=modems.length; i<j; i++) {
                console.warn('sim', i, modems[i]);
            }
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
            console.warn('sim1 loaded, loading CellularSingleSim.qml into cellData');
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
            console.warn('sim2 loaded, loading CellularDualSim.qml into cellData');
        }
    }

    OfonoNetworkRegistration {
        id: netReg
        modemPath: manager.modems[0] || ""
        onStatusChanged: {
            console.warn ("netReg onStatusChanged: " + status, modemPath);
        }
        onModeChanged: {
            console.warn ("netReg onModeChanged: " + mode, modemPath);
        }
    }

    Flickable {
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            Loader {
                id: cellData
                anchors.left: parent.left
                anchors.right: parent.right
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
                objectName: "chooseCarrier"
                value: {
                    if (root.state === "singleSim") {
                        return netReg.name ? netReg.name : i18n.tr("N/A")
                    } else {
                        return ''
                    }
                }
                progression: enabled
                onClicked: {
                    if (root.state === 'singleSim') {
                        pageStack.push(Qt.resolvedUrl("PageChooseCarrier.qml"), {
                            netReg: netReg,
                            title: i18n.tr("Carrier")
                        })
                    } else if (root.state === 'dualSim') {
                        pageStack.push(Qt.resolvedUrl("PageChooseCarriers.qml"), {
                            netReg: netReg,
                            sim1: sim1,
                            sim2: sim2
                        });
                    }
                }
            }

            ListItem.Standard {
                text: i18n.tr("APN")
                progression: true
                visible: showAllUI
            }
        }
    }

    GSettings {
        id: systemSettingsSettings
        schema.id: "com.ubuntu.touch.system-settings"
        // onChanged: {
        //     if (key == "modemPathToSimName") {

        //     }
        // }
        // Component.onCompleted: {
        //     console.warn('GSettings completed:', systemSettingsSettings.modemPathToSimName);

        //     for (var i=0, j=manager.modems.length; i < j; i++) {
        //         if(manager.modems[i] in systemSettingsSettings.modemPathToSimName) {
        //             if (sim1.path === manager.modems[i]) {
        //                 sim1.name = systemSettingsSettings.modemPathToSimName[manager.modems[i]];
        //                 console.warn('setting sim1 name to', systemSettingsSettings.modemPathToSimName[manager.modems[i]]);
        //             }
        //             if (sim2.path === manager.modems[i]) {
        //                 sim2.name = systemSettingsSettings.modemPathToSimName[manager.modems[i]];
        //                 console.warn('setting sim2 name to', systemSettingsSettings.modemPathToSimName[manager.modems[i]]);
        //             }
        //         }
        //     }
        // }
    }

}
