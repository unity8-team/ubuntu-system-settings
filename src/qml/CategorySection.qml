/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015-2016 Canonical Ltd.
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
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3


Column {
    anchors {
        left: parent.left
        right: parent.right
    }
    spacing: units.gu(1)

    property string category
    property string categoryName

    objectName: "categoryGrid-" + category

    SettingsItemTitle {
        id: header
        text: categoryName
        visible: repeater.count > 0
    }

    Column {
        id: col
        anchors {
            left: parent.left
            right: parent.right
        }

        Repeater {
            id: repeater

            model: pluginManager.itemModel(category)

            delegate: Loader {
                id: loader
                anchors {
                    left: col.left
                    right: col.right
                }
                sourceComponent: model.item.entryComponent
                active: model.item.visible
                Connections {
                    ignoreUnknownSignals: true
                    target: loader.item
                    onClicked: {
                        var pageComponent = model.item.pageComponent
                        var incubator;
                        if (pageComponent) {
                            Haptics.play();
                            incubator = apl.addPageToNextColumn(apl.primaryPage, model.item.pageComponent, {
                                plugin: model.item, pluginManager: pluginManager,
                                // pageStack: apl.primaryPage.pageStack
                            });
                            // if (incubator && incubator.status == Component.Loading) {
                            //     incubator.onStatusChanged = function(status) {
                            //         if (status == Component.Ready) {
                            //             apl.currentPanel = incubator.object;
                            //             incubator.object.Component.destruction.connect(function() {
                            //                 apl.currentPanel = null;
                            //             });
                            //         }
                            //     }
                            // }
                        }
                    }
                }
            }
        }
    }
}
