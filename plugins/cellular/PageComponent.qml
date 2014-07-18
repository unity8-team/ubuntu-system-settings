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

    property var sims: [sim1]

    states: [
        State {
            name: "singleSim"
            PropertyChanges { target: cellData; source: "Components/CellularSingleSim.qml" }
            when: manager.modems.length === 1
        },
        State {
            name: "dualSim"
            PropertyChanges { target: cellData; source: "Components/CellularDualSim.qml" }
            // dynamically load ofono bindings for second sim
            PropertyChanges { target: secondSimLoader; source: "Components/Sim.qml" }
            when: manager.modems.length === 2
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
            console.warn('manager complete with', modems.length, 'modems');
        }
    }

    // ofono bindings of first sim
    Sim {
        id: sim1
        // TODO: replace with user chosen name
        name: "SIM 1"
        path: manager.modems[0]
    }

    // ofono bindings for a second sim, dynamically loaded
    Loader {
        id: secondSimLoader
        onLoaded: {
            secondSimPath.target = secondSimLoader.item;
            secondSimName.target = secondSimLoader.item;
            root.sims = [sim1, secondSimLoader.item]
        }
    }

    // if second modem, bind its path to sim2
    Binding {
        id: secondSimPath
        property: "path"
        value: manager.modems[1]
    }

    // if second modem, bind its name to sim2
    Binding {
        id: secondSimName
        property: "name"
        value: "SIM 2"
    }

    OfonoNetworkRegistration {
        id: netReg
        modemPath: manager.modems[0]
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

                // bind cellData.sims to root.sims
                Component.onCompleted: {
                    simBinder.target = cellData.item
                    roamingBinder.target = cellData.item
                    roamingControlBinder.target = cellData.item
                }
            }

            Binding {
                id: simBinder
                property: "sims"
                value: sims
            }

            Binding {
                id: roamingBinder
                property: "dataEnabled"
                value: dataRoamingItem.enabled
            }

            Binding {
                id: roamingControlBinder
                property: "roamingAllowed"
                value: dataRoamingControl.checked
            }

            ListItem.Standard {
                id: dataRoamingItem
                objectName: "dataRoamingSwitch"
                text: i18n.tr("Data roaming")
                // sensitive to data type, and disabled if "Off" is selected
                control: Switch {
                    id: dataRoamingControl
                    onClicked: cellData.item.roamingAllowed = checked
                }
            }

            // ListItem.SingleValue {
            //     text : i18n.tr("Hotspot disabled because Wi-Fi is off.")
            //     visible: showAllUI && !hotspotItem.visible
            // }

            // ListItem.SingleValue {
            //     id: hotspotItem
            //     text: i18n.tr("Wi-Fi hotspot")
            //     progression: true
            //     onClicked: {
            //         pageStack.push(Qt.resolvedUrl("Hotspot.qml"))
            //     }
            //     visible: showAllUI && (actionGroup.actionObject.valid ? actionGroup.actionObject.state : false)
            // }

            // ListItem.Standard {
            //     text: i18n.tr("Data usage statistics")
            //     progression: true
            //     visible: showAllUI
            // }

            ListItem.SingleValue {
                text: i18n.tr("Carrier", "Carriers", manager.modems.length);
                objectName: "chooseCarrier"
                value: {
                    if (root.state === 'singleSim') {
                        return netReg.name ? netReg.name : i18n.tr("N/A")
                    } else {
                        return ''
                    }
                }
                progression: enabled
                onClicked: {
                    if (root.state === 'singleSim') {
                        pageStack.push(Qt.resolvedUrl("ChooseCarrier.qml"), {
                            netReg: netReg,
                            title: i18n.tr("Carrier")
                        })
                    } else if (root.state === 'dualSim') {
                        pageStack.push(Qt.resolvedUrl("ChooseCarriers.qml"), {
                            netReg: netReg,
                            sim1: sims[0],
                            sim2: sims[1]
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
}
