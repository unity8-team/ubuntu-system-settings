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

    Component.onCompleted: pageStack.push(mainPage)

    PluginManager {
        id: pluginManager
    }

    PageStack {
        id: pageStack

        Page {
            id: mainPage
            title: i18n.tr("System Settings")
            visible: false

            Column {
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: units.gu(3)

                UncategorizedItemsView {
                    model: pluginManager.itemModel("uncategorized-top")
                }

                ListItem.Divider {}

                CategoryGrid {
                    category: "personal"
                }

                CategoryGrid {
                    category: "network"
                }

                CategoryGrid {
                    category: "system"
                }

                ListItem.Divider {}

                UncategorizedItemsView {
                    model: pluginManager.itemModel("uncategorized-bottom")
                }
            }
        }
    }
}
