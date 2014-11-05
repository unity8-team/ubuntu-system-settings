/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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
 *
 * This element looks like a OptionSelectorDelegate.
 *
 */
import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem

Rectangle {
    property string operatorName
    anchors {
        margins: units.gu(2)
        left: parent.left
        right: parent.right
    }
    height: 0
    clip: true
    color: Theme.palette.normal.background
    Label {
        clip: true
        text: operatorName || i18n.tr("None")
        verticalAlignment: Text.AlignVCenter
        anchors.fill: parent
        ListItem.ThinDivider { anchors.bottom: parent.bottom }
    }

    Behavior on height {
        NumberAnimation {
            duration: UbuntuAnimation.SnapDuration
        }
    }
}
