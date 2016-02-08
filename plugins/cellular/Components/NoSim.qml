/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/
import QtQuick 2.4
import Ubuntu.Components 1.3

Column {

    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
        margins: units.gu(8)
    }

    spacing: units.gu(2)

    Label {
        anchors { left: parent.left; right: parent.right; }
        text: i18n.tr("No SIM detected")
        fontSize: "large"
        horizontalAlignment: Text.AlignHCenter
        color: UbuntuColors.lightGrey
        wrapMode: Text.WordWrap
    }

    Label {
        anchors { left: parent.left; right: parent.right; }
        text: i18n.tr("Insert a SIM, then restart the device.")
        horizontalAlignment: Text.AlignHCenter
        color: UbuntuColors.lightGrey
        wrapMode: Text.WordWrap
    }
}
