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
 
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

Column {
    id: root
    anchors {
        left: parent.left
        right: parent.right
        leftMargin: units.gu(2)
        rightMargin: units.gu(2)
    }
    spacing: units.gu(1)
    height: childrenRect.height
    property string text
    ListItem.Standard {
        height: mouseLabel.height + units.gu(1)
        Label {
            id: mouseLabel
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            text: root.text
            /* We are "large" to avoid looking like normal "medium" text */
            fontSize: "large"
            elide: Text.ElideRight
            color: Theme.palette.normal.baseText
            font.weight: Text.Normal
        }
        highlightWhenPressed: false
        showDivider: false
    }
}
