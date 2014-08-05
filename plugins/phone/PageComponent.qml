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

ItemPage {
    id: root
    title: i18n.tr("Phone")
    flickable: flick

    property var modemsSorted: manager.modems.slice(0).sort()

    states: [
        State {
            name: "singleSim"
            PropertyChanges {
                target: singleSim
                source: "SingleSim.qml"
            }
        },
        State {
            name: "dualSim"
            PropertyChanges {
                target: dualSim
                source: "DualSim.qml"
            }
        }
    ]

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

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            Loader {
                id: singleSim
                anchors.left: parent.left
                anchors.right: parent.right
            }

            Loader {
                id: dualSim
                anchors.left: parent.left
                anchors.right: parent.right
            }
        }
    }
}
