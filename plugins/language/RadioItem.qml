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

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItems

ListItems.Empty {
    id: root

    property bool active: false

    Label {
        text: root.text

        anchors.left: parent.left
        anchors.right: radio.left
        anchors.margins: units.gu(2)
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        id: radio

        color: root.active ? "grey" : "transparent"
        width: units.gu(1)
        height: 1.0 * width
        radius: 0.5 * width

        anchors.right: parent.right
        anchors.margins: units.gu(2)
        anchors.verticalCenter: parent.verticalCenter
    }
}
