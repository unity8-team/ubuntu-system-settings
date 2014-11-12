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
import "Components" as LocalComponents
import "sims.js" as Sims

ItemPage {
    id: root
    title: i18n.tr("Cellular")
    objectName: "cellularPage"

    property var modemsSorted: manager.modems.slice(0).sort()
    property int simsLoaded: 0

    states: [
        State {
            name: "noSim"
            when: (simsLoaded === 0) || (Sims.getPresentCount() === 0)
            StateChangeScript {
                script: loader.source = "Components/NoSim.qml"
            }
        },
        State {
            name: "singleSim"
            StateChangeScript {
                script: loader.setSource("Components/SingleSim.qml", {
                    sim: Sims.getFirstPresent()
                })
            }
            when: simsLoaded && (Sims.getPresentCount() === 1)
        },
        State {
            name: "multiSim"
            StateChangeScript {
                script: loader.setSource("Components/MultiSim.qml", {
                    sims: Sims.getAll(),
                    modems: modemsSorted
                })
            }
            when: simsLoaded && (Sims.getPresentCount() > 1)
        }
    ]

    OfonoManager {
        id: manager
        Component.onCompleted: {
            var component = Qt.createComponent("Components/Sim.qml");
            modemsSorted.forEach(function (path) {
                var sim = component.createObject(root, {
                    path: path
                });
                if (sim === null) {
                    console.warn('Failed to create Sim qml:',
                        component.errorString());
                } else {
                    Sims.add(sim);
                }
            });
        }
    }

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

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        Column {
            anchors { left: parent.left; right: parent.right }
            Loader {
                id: loader
                anchors { left: parent.left; right: parent.right }
            }
        }
    }
}
