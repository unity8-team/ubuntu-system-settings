/*
 * Copyright (C) 2014 Canonical, Ltd.
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
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem

Item {
    property alias text: label.text
    property alias image: image.source

    signal clicked(variant mouse)

    height: units.gu(6)

    Item {

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        ListItem.ThinDivider {
            anchors {
                top: parent.top
            }
        }

        Label {
            id: label
            anchors {
                left: parent.left
                leftMargin: units.gu(2)
                verticalCenter: parent.verticalCenter
            }

            color: "grey" // TODO karni: Update Ubuntu.Compoonents.Themes.Palette.
            font.family: "Ubuntu"
            fontSize: "medium"
            elide: Text.ElideRight
            textFormat: Text.PlainText
            width: parent.width - image.width - image.leftMargin - anchors.leftMargin
        }

        Image {
            id: image
            readonly property double leftMargin: units.gu(1)
            x: label.x + label.contentWidth + leftMargin
            anchors {
                verticalCenter: parent.verticalCenter
            }
            height: units.gu(2.1)
            fillMode: Image.PreserveAspectFit
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: parent.clicked(mouse)
    }
}
