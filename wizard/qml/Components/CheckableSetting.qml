/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QMenuModel 0.1
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

ListItem.Empty {
    id: listItem

    property alias text: label.text
    property alias checked: checkBox.checked
    property real leftMargin
    property real rightMargin

    implicitHeight: label.height + units.gu(2)

    onClicked: checked = !checked

    Item {
        anchors.fill: parent

        CheckBox {
            id: checkBox
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                leftMargin: listItem.leftMargin
            }
        }

        Label {
            id: label
            anchors {
                left: checkBox.right
                right: parent.right
                verticalCenter: parent.verticalCenter
                leftMargin: units.gu(2)
                rightMargin: listItem.rightMargin
            }
            wrapMode: Text.WordWrap
        }
    }
}
