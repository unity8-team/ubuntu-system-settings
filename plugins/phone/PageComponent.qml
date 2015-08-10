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

import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.SystemSettings.Sound 1.0
import MeeGo.QOfono 0.2
import "sims.js" as Sims

ItemPage {
    id: root
    objectName: "phonePage"
    title: i18n.tr("Phone")
    flickable: flick

    property var modemsSorted: []
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
                script: loader.setSource("SingleSim.qml", {
                    sim: Sims.getFirstPresent()
                })
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
        onModemsChanged: {
            root.modemsSorted = modems.slice(0).sort();
            Sims.createQML();
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
                    property bool serverChecked: soundPlugin.dialpadSoundsEnabled
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: soundPlugin.dialpadSoundsEnabled = checked
                }
                text: i18n.tr("Dialpad tones")
            }
        }
    }
}
