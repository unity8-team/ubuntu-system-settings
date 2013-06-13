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
import SystemSettings 1.0
import Ubuntu.Components.ListItems 0.1 as ListItem

ItemPage {
    id: root

    title: i18n.tr("Reset phone")
    flickable: scrollWidget

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: columnId.height

        Column {
            id: columnId
            spacing: units.gu(1)

            anchors.left: parent.left
            anchors.right: parent.right

            Item { height: units.gu(1); width: parent.width }

            Button {
                anchors {
                    margins: units.gu(3)
                    left: parent.left
                    right: parent.right
                }
                text: i18n.tr("Reset launcher & home screen…")
            }
            Button {
                anchors {
                    margins: units.gu(3)
                    left: parent.left
                    right: parent.right
                }
                text: i18n.tr("Reset all system settings…")
            }
            Button {
                anchors {
                    margins: units.gu(3)
                    left: parent.left
                    right: parent.right
                }
                text: i18n.tr("Erase and reset everything…")
            }
        }
    }
}
