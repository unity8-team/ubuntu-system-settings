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
    property variant modelIndexVar: modelIndex
    anchors.left: parent.left
    anchors.right: parent.right
    height: col.height
    color: "green"

    FlatModel {
        id: sectionModel
        model: menuModel
        rootIndex: modelIndexVar
    }

    Column {
        id: col
        anchors.left: parent.left
        anchors.right: parent.right

        Repeater {
            model: sectionModel
            delegate: MenuItemDelegate {
                anchors.left: parent.left
                anchors.right: parent.right
            }
        }
    }
}
