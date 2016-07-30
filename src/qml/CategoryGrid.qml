/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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
    anchors {
        left: parent.left
        right: parent.right
    }
    spacing: units.gu(1)

    property string category
    property string categoryName

    objectName: "categoryGrid-" + category

    ListItem.Standard {
        id: header

        highlightWhenPressed: false
        showDivider: false
        text: categoryName
        visible: repeater.count > 0
    }

    Grid {
        property int itemWidth: units.gu(12)

        // The amount of whitespace, including column spacing
        property int space: parent.width - columns * itemWidth

        // The column spacing is 1/n of the left/right margins
        property int n: 4

        columnSpacing: space / ((2 * n) + (columns - 1))
        rowSpacing: units.gu(3)
        width: (columns * itemWidth) + columnSpacing * (columns - 1)
        anchors.horizontalCenter: parent.horizontalCenter
        columns: {
            var items = Math.floor(parent.width / itemWidth)
            var count = repeater.count
            return count < items ? count : items
        }

        Repeater {
            id: repeater

            model: pluginManager.itemModel(category)

            delegate: Loader {
                id: loader
                width: parent.itemWidth
                sourceComponent: model.item.entryComponent
                active: model.item.visible
                Connections {
                    ignoreUnknownSignals: true
                    target: loader.item
                    onClicked: {
                        var pageComponent = model.item.pageComponent
                        if (pageComponent) {
                            pageStack.push(model.item.pageComponent,
                                           { plugin: model.item, pluginManager: pluginManager })
                        }
                    }
                }
            }
        }
    }
    ListItem.ThinDivider { visible: header.visible }
}
