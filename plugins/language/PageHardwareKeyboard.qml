/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
 *          Jonas G. Drange <jonas.drange@canonical.com>
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
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.Settings.Components 0.1 as USC
import Ubuntu.SystemSettings.LanguagePlugin 1.0

ItemPage {
    id: root
    objectName: "hwKbdPage"

    title: i18n.tr("Hardware keyboard")
    flickable: scrollWidget

    Component {
        id: keyboardLayouts

        KeyboardLayouts {}
    }

    HardwareKeyboardPlugin {
        id: plugin
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: contentHeight > root.height ?
                        Flickable.DragAndOvershootBounds :
                        Flickable.StopAtBounds
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors.left: parent.left
            anchors.right: parent.right


            ListItem.SingleValue {
                text: i18n.tr("Layouts and other sources")
                value: plugin.keyboardLayoutsModel.subset.length == 1 ?
                       plugin.keyboardLayoutsModel.superset[plugin.keyboardLayoutsModel.subset[0]][0] :
                       plugin.keyboardLayoutsModel.subset.length
                progression: true

                onClicked: pageStack.addPageToNextColumn(root,
                    Qt.resolvedUrl("KeyboardLayouts.qml"), {
                        plugin: plugin,
                        currentLayoutsDraggable: true
                    }
                )
            }
        }
    }
}
