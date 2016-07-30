/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015-2016 Canonical Ltd.
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

import Ubuntu.Components 1.3

ListItem {
    id: root
    default property alias slots: layoutItem.children
    property alias         layout: layoutItem
    property string        text
    property bool          showDivider: true
    property bool          highlightWhenPressed: true

    height: layoutItem.height + (divider.visible ? divider.height : 0)
    divider.visible: showDivider
    divider.colorFrom: "#EEEEEE"
    divider.colorTo: "#EEEEEE"
    divider.height: units.dp(1)
    highlightColor: highlightWhenPressed ? undefined : "transparent"

    ListItemLayout {
        id: layoutItem
        title.text: root.text
        title.opacity: enabled ? 1 : 0.5
    }
}
