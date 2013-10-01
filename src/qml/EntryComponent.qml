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

Item {
    id: root

    signal clicked

    height: col.height

    Column {
        id: col
        anchors.left: parent.left
        anchors.right: parent.right

        Image {
            id: icon
            anchors.horizontalCenter: parent.horizontalCenter
            width: units.gu(6)
            height: width
            source: model.icon
        }
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: i18n.dtr(model.item.translations, model.displayName)
            width: col.width
            horizontalAlignment: Text.AlignHCenter
            fontSize: "small"
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
