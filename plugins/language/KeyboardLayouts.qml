/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.LanguagePlugin 1.0

ItemPage {
    id: root

    title: i18n.tr("Keyboard layouts")

    UbuntuLanguagePlugin {
        id: plugin
    }

    Flickable {
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        ListItem.Standard {
            id: currentLayoutsHeader
            text: i18n.tr("Current layouts:")

            anchors.left: parent.left
            anchors.right: parent.right
        }

        ListView {
            id: currentLayouts

            interactive: false

            anchors.top: currentLayoutsHeader.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            height: contentItem.childrenRect.height
            contentHeight: contentItem.childrenRect.height

            model: plugin.currentPlugins.length
            delegate: SettingsCheckEntry {
                textEntry: plugin.plugins[plugin.currentPlugins[index]]
                checkStatus: true
            }
        }

        ListItem.Standard {
            id: availableLayoutsHeader
            text: i18n.tr("All layouts available:")

            anchors.top: currentLayouts.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }

        ListView {
            id: availableLayouts

            interactive: false

            anchors.top: availableLayoutsHeader.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            height: contentItem.childrenRect.height
            contentHeight: contentItem.childrenRect.height

            model: plugin.plugins.length
            delegate: SettingsCheckEntry {
                textEntry: plugin.plugins[index]
                checkStatus: false
            }
        }
    }
}
