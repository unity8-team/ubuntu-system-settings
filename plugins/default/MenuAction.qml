/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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
import Ubuntu.Components 0.1
import Ubuntu.SystemSettings.Utils 0.1

Rectangle {
    id: root
    height: 40 // FIXME
    anchors.left: parent.left
    anchors.right: parent.right
    color: "lightsteelblue"

    Text {
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        text: "%1 (%2)".arg(label).arg(action)
    }

    MouseArea {
        anchors.fill: parent
        onClicked: console.log("action triggered: " + action)
    }
}
