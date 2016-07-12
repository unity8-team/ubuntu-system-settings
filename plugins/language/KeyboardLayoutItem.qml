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

ListItem {
    property string name
    property alias  checked: checkBox.checked
    property alias  shortName: shortName.text
    property alias draggable: dragHandle.visible
    property alias dragger: dragArea.drag

    signal dragStarted()
    signal dragFinished()

    height: layout.height + divider.height

    ListItemLayout {
        id: layout

        title.text: name

        Rectangle {
            width: units.gu(3.0)
            height: units.gu(3.0)
            radius: units.gu(0.5)

            color: Theme.palette.normal.backgroundText

            Label {
                id: shortName

                color: Theme.palette.normal.background
                fontSize: "small"

                anchors.centerIn: parent
            }

            SlotsLayout.position: SlotsLayout.First
        }

        Icon {
            id: dragHandle
            width: units.gu(2.5)
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
            SlotsLayout.position: SlotsLayout.Leading
        }

        CheckBox {
            id: checkBox
            SlotsLayout.position: SlotsLayout.Trailing
        }
    }
    onClicked: checked = !checked
}
