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
import Ubuntu.Components 0.1
import Ubuntu.Settings.Menus 0.1 as Menus

Item {
    id: sectionHeader

    property alias text: label.text
    property real leftMargin
    property real rightMargin

    implicitHeight: label.height + separator.height + units.gu(2)

    Label {
        id: label
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: -separator.height / 2
            leftMargin: sectionHeader.leftMargin
            rightMargin: sectionHeader.rightMargin
        }
        wrapMode: Text.WordWrap
        fontSize: "large"
    }

    Menus.SeparatorMenu {
        id: separator
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: units.dp(2)
    }
}
