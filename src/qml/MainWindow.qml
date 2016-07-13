/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013, 2014, 2015 Canonical Ltd.
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
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import SystemSettings 1.0

MainView {
    id: main
    implicitWidth: units.gu(50)
    implicitHeight: units.gu(90)
    applicationName: "ubuntu-system-settings"
    objectName: "systemSettingsMainView"
    automaticOrientation: true
    anchorToKeyboard: true

    function loadPluginByName(pluginName, pluginOptions) {
        var plugin = pluginManager.getByName(pluginName)
        var opts = { plugin: plugin,
                     pluginManager: pluginManager }

        if (pluginOptions)
            opts.pluginOptions = pluginOptions

        if (plugin) {
            // Got a valid plugin name - load it
            var pageComponent = plugin.pageComponent
            if (pageComponent) {
                while (pageStack.depth > 1)
                    pageStack.pop()
                pageStack.push(pageComponent, opts)
            }
            return true
        } else {
            // Invalid plugin
            console.log("Plugin " + pluginName + " does not exist.")
            return false
        }
    }

    Component.onCompleted: {
        i18n.domain = "ubuntu-system-settings"
        i18n.bindtextdomain("ubuntu-system-settings", i18nDirectory)
        pageStack.push(mainPage)
        if (defaultPlugin) {
            if (!loadPluginByName(defaultPlugin, pluginOptions))
                Qt.quit()
        }

        // when running in windowed mode, constrain width
        view.minimumWidth  = Qt.binding( function() { return units.gu(40) } )
        view.maximumWidth = Qt.binding( function() { return units.gu(50) } )
    }

    Connections {
        target: UriHandler
        onOpened: {
            var url = String(uris);
            url = Utilities.mapUrl(url);
            var panelAndOptions = url.replace("settings:///system/", "")
            var optionIndex = panelAndOptions.indexOf("?")
            var panel = optionIndex > -1 ?
                        panelAndOptions.substring(0, optionIndex) :
                        panelAndOptions
            var urlParams = {}
            // Parse URL options
            // From http://stackoverflow.com/a/2880929
            if (optionIndex > -1) { // Got options
                var match,
                    // Regex for replacing addition symbol with a space
                    pl     = /\+/g,
                    search = /([^&=]+)=?([^&]*)/g,
                    decode = function (s) {
                        return decodeURIComponent(s.replace(pl, " "))
                    }
                while (match = search.exec(
                           panelAndOptions.substring(optionIndex + 1)))
                       urlParams[decode(match[1])] = decode(match[2])

                loadPluginByName(panel, urlParams)
            } else {
                loadPluginByName(panel)
            }

        }
    }

    PluginManager {
        id: pluginManager
    }

    PageStack {
        id: pageStack

        Page {
            id: mainPage
            objectName: "systemSettingsPage"
            title: i18n.tr("System Settings")
            visible: false
            flickable: mainFlickable

            head.actions: [
                Action {
                    objectName: "searchAction"
                    iconName: "find"
                    onTriggered: {
                        pluginManager.filter = "";
                        search.visible = !search.visible;
                    }
                }
            ]

            Flickable {
                id: mainFlickable
                anchors.fill: parent
                contentHeight: contentItem.childrenRect.height
                boundsBehavior: (contentHeight > mainPage.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
                /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
                   otherwise the UI might end up in a situation where scrolling doesn't work */
                flickableDirection: Flickable.VerticalFlick

                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    SettingsListItems.SingleControl {
                        id: search
                        visible: false
                        TextField {
                            id: searchField
                            width: parent.width - units.gu(4)
                            placeholderText: i18n.tr("Search")
                            objectName: "searchTextField"
                            inputMethodHints: Qt.ImhNoPredictiveText
                            onDisplayTextChanged:
                                pluginManager.filter = displayText
                        }
                        onVisibleChanged: if (visible) searchField.forceActiveFocus()
                    }

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
