/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
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
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

ListItem.Empty {
    id: root
    property alias name: name.text
    property alias checked: checkBox.checked
    property alias shortName: shortName.text
    property alias draggable: dragHandle.visible
    property alias dragger: dragArea.drag

    signal dragStarted()
    signal dragFinished()

    onClicked: {
        checkBox.checked = !checkBox.checked
    }

    Rectangle {
        id: icon
        anchors {
            left: parent.left
            leftMargin: units.gu(2)
        }
        width: units.gu(3.0)
        height: units.gu(3.0)
        radius: units.gu(0.5)

        color: Theme.palette.normal.backgroundText

        anchors.verticalCenter: parent.verticalCenter

        Label {
            id: shortName

            color: Theme.palette.normal.background
            fontSize: "small"

            anchors.centerIn: parent
        }
    }

    Label {
        id: name
        anchors {
            left: icon.right
            leftMargin: units.gu(2)
            right: dragHandle.visible ? dragHandle.left : checkBox.left
            rightMargin: units.gu(3)
        }
        elide: Text.ElideMiddle
        anchors.verticalCenter: parent.verticalCenter
    }

    Icon {
        id: dragHandle
        width: units.gu(2.5)
        height: parent.height
        anchors {
            right: checkBox.left
            rightMargin: units.gu(3)
            verticalCenter: parent.verticalCenter
        }

        MouseArea {
            id: dragArea
            anchors.fill: parent

            onPressed: root.dragStarted()
            onReleased: root.dragFinished()
        }

        name: "grip-large"
    }

    CheckBox {
        id: checkBox
        anchors {
            right: parent.right
            rightMargin: units.gu(2)
            verticalCenter: parent.verticalCenter
        }
    }
}
