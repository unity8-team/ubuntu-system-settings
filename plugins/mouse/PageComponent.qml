/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Ken VanDine <ken.vandine@canonical.com>
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
import QtSystemInfo 5.5
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

ItemPage {
    id: root
    title: i18n.tr("Mouse & Touchpad")
    objectName: "mousePage"
    flickable: scrollWidget

    InputDeviceManager {
        id: miceModel
        filter: InputInfo.Mouse
    }

    InputDeviceManager {
        id: touchpadsModel
        filter: InputInfo.TouchPad
    }

    property bool connected: miceModel.count > 0 || touchpadsModel.count > 0

    states: [
        State {
            name: "none"
            when: !connected
            StateChangeScript {
                script: loader.source = "None.qml"
            }
        },
        State {
            name: "mouseConnected"
            StateChangeScript {
                script: loader.setSource("Connected.qml", {})
            }
            when: connected
        }
    ]

    Flickable {
        id: scrollWidget
        anchors {
            fill: parent
            topMargin: units.gu(1)
            bottomMargin: units.gu(1)
        }
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
