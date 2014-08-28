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
import Ubuntu.Components 1.1
import Ubuntu.Settings.Components 0.1

Item {
    id: root

    signal clicked

    height: button.height

    objectName: "entryComponent-" + model.item.baseName

    AbstractButton {
        id: button
        anchors.left: parent.left
        anchors.right: parent.right
        onClicked: root.clicked()

        height: col.height

        Column {
            id: col
            anchors.left: parent.left
            anchors.right: parent.right

            Icon {
                id: icon
                anchors.horizontalCenter: parent.horizontalCenter
                height: units.gu(4)
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
    }

    UbuntuShape {
        z: -1
        visible: button.pressed
        anchors{
            fill: root
            margins: -units.gu(0.25)
        }
        color: UbuntuColors.darkGrey
        opacity: 0.15
    }
}
