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
import Ubuntu.Components.ListItems 0.1 as ListItem
import SystemSettings 1.0

Column {
    property alias model: repeater.model

    visible: repeater.count > 0

    anchors.left: parent.left
    anchors.right: parent.right

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
                Connections {
                    ignoreUnknownSignals: true
                    target: loader.item
                    onClicked: {
                        var pageComponent = model.item.pageComponent
                        if (pageComponent) {
                            pageStack.push(model.item.pageComponent,
                                           { plugin: model.item,
                                             pluginManager: pluginManager })
                        }
                    }
                }
            }
        }
    }
}
