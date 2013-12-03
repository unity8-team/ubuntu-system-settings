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
 * Ken VanDine <ken.vandine@canonical.com>
 *
*/

import QtQuick 2.0
import GSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Column {
    id: wallpaperGrid
    anchors.left: parent.left
    anchors.right: parent.right
    height: childrenRect.height
    spacing: units.gu(2)

    property var model
    property int columns
    property int itemWidth: (mainPage.width * 0.7) / columns
    property int itemHeight: (mainPage.height / mainPage.width) * itemWidth
    property string title
    signal selected (string uri)

    ListItem.Standard {
        anchors.left: parent.left
        anchors.right: parent.right
        text: title
        showDivider: false
    }

    Grid {
        anchors.horizontalCenter: parent.horizontalCenter
        columns: wallpaperGrid.columns
        spacing: itemWidth * 0.3
        height: childrenRect.height
        Repeater {
            model: wallpaperGrid.model
            Image {
               id: itemImage
               source: modelData
               width: itemWidth
               height: itemHeight
               sourceSize.width: width
               sourceSize.height: height
               fillMode: Image.PreserveAspectCrop
               asynchronous: true

                ActivityIndicator {
                    anchors.centerIn: parent
                    running: parent.status === Image.Loading
                    visible: running
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        selected(modelData);
                    }
                }
            }
        }
    }
}
