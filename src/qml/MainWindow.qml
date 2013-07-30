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

MainView {
    width: units.gu(48)
    height: units.gu(90)
    applicationName: "SystemSettings"
    automaticOrientation: true

    Component.onCompleted: {
        i18n.domain = "ubuntu-system-settings"
        if (defaultPlugin) {
            var plugin = pluginManager.getByName(defaultPlugin)
            if (plugin) {
                // Got a valid plugin name - load it
                var pageComponent = plugin.pageComponent
                if (pageComponent)
                    pageStack.push(pageComponent, { plugin: plugin, pluginManager: pluginManager })
            } else {
                // Invalid plugin passed on the commandline
                console.log("Plugin " + defaultPlugin + " does not exist.")
                Qt.quit()
            }
        } else {
            pageStack.push(mainPage)
        }
    }

    PluginManager {
        id: pluginManager
    }

    PageStack {
        id: pageStack

        Page {
            id: mainPage
            title: i18n.tr("System Settings")
            visible: false
            flickable: mainFlickable

            Flickable {
                id: mainFlickable
                anchors.fill: parent
                contentHeight: contentItem.childrenRect.height
                boundsBehavior: (contentHeight > mainPage.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    UncategorizedItemsView {
                        model: pluginManager.itemModel("uncategorized-top")
                    }

                    CategoryGrid {
                        category: "network"
                        categoryName: i18n.tr("Network")
                    }

                    CategoryGrid {
                        category: "personal"
                        categoryName: i18n.tr("Personal")
                    }

                    CategoryGrid {
                        category: "system"
                        categoryName: i18n.tr("System")
                    }

                    UncategorizedItemsView {
                        model: pluginManager.itemModel("uncategorized-bottom")
                    }
                }
            }
        }
    }
}
