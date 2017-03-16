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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import SystemSettings 1.0

Column {
    id: column
    property alias model: repeater.model

    visible: repeater.count > 0

    anchors.left: parent.left
    anchors.right: parent.right

    function getPluginIndexByName(plugin) {
        return repeater.model.getIndexByName(plugin)
    }

    function getPluginNameByIndex(index) {
        return repeater.model.getNameByIndex(index)
    }

    function getPreviousPluginIndex(from) {
        return repeater.model.getPreviousVisibleIndex(from)
    }

    function getNextPluginIndex(from) {
        return repeater.model.getNextVisibleIndex(from)
    }

    Repeater {
        id: repeater
        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            Loader {
                id: loader
                anchors.left: parent.left
                anchors.right: parent.right
                sourceComponent: model.item.entryComponent
                active: model.item.visible
                Connections {
                    ignoreUnknownSignals: true
                    target: loader.item
                    onClicked: {
                        var pageComponent = model.item.pageComponent
                        if (pageComponent) {
                            Haptics.play();
                            loadPluginByName(model.item.baseName);
                            if (apl.columns == 1) {
                                currentPluginPage.forceActiveFocus();
                            }
                        }
                    }
                }
                Binding {
                    target: loader.item
                    property: "color"
                    value: theme.palette.highlighted.background
                    when: currentPlugin == model.item.baseName
                }
                Binding {
                    target: loader.item
                    property: "color"
                    value: "transparent"
                    when: currentPlugin != model.item.baseName
                }
            }
        }
    }
}
