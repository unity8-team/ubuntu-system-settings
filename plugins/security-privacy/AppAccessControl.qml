/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Alberto Mardegan <alberto.mardegan@canonical.com>
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import SystemSettings 1.0

ItemPage {
    id: root

    property alias model: repeater.model
    property alias caption: captionLabel.text

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.Caption {
            id: captionLabel
        }

        Repeater {
            id: repeater

            ListItem.Standard {
                text: model.applicationName
                iconSource: model.iconName
                control: Switch {
                    id: welcomeStatsSwitch
                    checked: model.granted
                    onClicked: root.model.setEnabled(index, !model.granted)
                }
            }
        }
    }
}
