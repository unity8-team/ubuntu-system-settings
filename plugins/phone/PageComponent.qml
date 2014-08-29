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
import Ubuntu.SystemSettings.Sound 1.0
import MeeGo.QOfono 0.2
import "sims.js" as Sims

ItemPage {
    id: root
    objectName: "phonePage"
    title: i18n.tr("Phone")
    flickable: flick

    property var modemsSorted: manager.modems.slice(0).sort()
    property var simsLoaded: 0

    states: [
        State {
            name: "noSim"
            StateChangeScript {
                script: loader.setSource("NoSims.qml")
            }
            when: (simsLoaded === 0) || (Sims.getPresentCount() === 0)
        },
        State {
            name: "singleSim"
            StateChangeScript {
                script: loader.setSource("SingleSim.qml", { sim: Sims.get(0) })

            }
            when: simsLoaded && (Sims.getPresentCount() === 1)
        },
        State {
            name: "multiSim"
            StateChangeScript {
                script: loader.setSource("MultiSim.qml", {
                    sims: Sims.getAll()
                })
            }
            when: simsLoaded && (Sims.getPresentCount() > 1)
        }
    ]

    OfonoManager {
        id: manager
        Component.onCompleted: {
            // create ofono bindings for all modem paths
            var component = Qt.createComponent("Ofono.qml");
            modemsSorted.forEach(function (path) {
                var sim = component.createObject(root, {
                    path: path
                });
                if (sim === null) {
                    console.warn('failed to create sim object');
                } else {
                    Sims.add(sim);
                }
            });
        }
    }

    UbuntuSoundPanel { id: soundPlugin }

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
            Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors { left: parent.left; right: parent.right }

            Loader {
                id: loader
                anchors { left: parent.left; right: parent.right }
            }

            ListItem.Divider {}

            ListItem.Standard {
                control: Switch {
                    objectName: "dialpadSounds"
                    checked: soundPlugin.dialpadSoundsEnabled
                    onCheckedChanged: soundPlugin.dialpadSoundsEnabled = checked
                }
                text: i18n.tr("Dialpad sounds")
            }
        }
    }
}
