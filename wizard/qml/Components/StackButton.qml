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

import QtQuick 2.3
import Ubuntu.Components 1.1

Item {
    id: stackButton

    property string text
    property bool leftArrow: false
    property bool rightArrow: false

    signal clicked()

    readonly property bool hasArrow: leftArrow || rightArrow

    width: button.width
    height: button.height

    Button {
        id: button
        anchors.fill: parent
        visible: !hasArrow
        text: stackButton.text
        onClicked: stackButton.clicked()
    }

    Label {
        id: label
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        visible: hasArrow
        text: {
            var str = ""
            if (leftArrow)
                str += "〈  "
            str += stackButton.text
            if (rightArrow)
                str += "  〉"
            return str
        }
        horizontalAlignment: leftArrow ? Text.AlignLeft : Text.AlignRight

        MouseArea {
            anchors.fill: parent
            onClicked: stackButton.clicked()
        }
    }
}
