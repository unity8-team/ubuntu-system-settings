/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3

MainView {
    id: main
    implicitWidth: units.gu(140)
    implicitHeight: units.gu(90)
    applicationName: "ubuntu-system-settings"
    objectName: "systemSettingsMainView"
    automaticOrientation: true
    anchorToKeyboard: true
    focus: true
    property var pluginManager: PluginManager {}
    property string currentPlugin: ""

    /* Workaround for lp:1648801, i.e. APL does not support a placeholder,
    so we implement it here. */
    property string placeholderPlugin: "about"

    function loadPluginByName(pluginName, pluginOptions) {
        var plugin = pluginManager.getByName(pluginName)
        var opts = { plugin: plugin,
                     pluginManager: pluginManager }

        if (pluginOptions)
            opts.pluginOptions = pluginOptions

        if (plugin) {
            // Got a valid plugin name - load it
            var pageComponent = plugin.pageComponent
            var page;
            if (pageComponent) {
                apl.removePages(apl.primaryPage);
                page = apl.addComponentToNextColumnSync(
                    apl.primaryPage, pageComponent, opts
                );
                page.Component.destruction.connect(function () {
                    mainPage.forceActiveFocus()
                }.bind(plugin))
                page.forceActiveFocus()
                currentPlugin = pluginName;
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

        if (defaultPlugin) {
            if (!loadPluginByName(defaultPlugin, pluginOptions))
                Qt.quit()
        } else if (apl.columns > 1) {
            loadPluginByName(placeholderPlugin);
            aplConnections.target = apl;
        }

        // when running in windowed mode, constrain width
        view.minimumWidth  = Qt.binding( function() { return units.gu(40) } )
        view.maximumWidth = Qt.binding( function() { return units.gu(140) } )

        mainPage.forceActiveFocus()
    }

    Connections {
        id: aplConnections
        ignoreUnknownSignals: true
        onColumnsChanged: {
            var columns = target.columns;
            if (columns > 1) {
               if (!currentPlugin) {
                    loadPluginByName(placeholderPlugin);
                } else { 
                    loadPluginByName(currentPlugin);
                }
            } else if (columns == 1) {
                apl.removePages(apl.primaryPage);
            }
        }
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
    
    USSAdaptivePageLayout {
        id: apl
        objectName: "apl"
        anchors.fill: parent
        primaryPage: mainPage
        focus: true
        layouts: [
            PageColumnsLayout {
                when: width >= units.gu(90)
                PageColumn {
                    minimumWidth: units.gu(40)
                    maximumWidth: units.gu(50)
                    preferredWidth: units.gu(50)
                }
                PageColumn {
                    fillWidth: true
                }
            },
            PageColumnsLayout {
                when: true
                PageColumn {
                    fillWidth: true
                    minimumWidth: units.gu(40)
                }
            }
        ]

        Page {
            id: mainPage
            objectName: "systemSettingsPage"
            visible: false
            header: standardHeader
            focus: true

            function selectPreviousPlugin() {
                for (var i = 0; i < mainColumn.children.length; ++i) {
                    var curr = mainColumn.children[i].getPluginIndexByName(main.currentPlugin)

                    if (curr >= 0) {
                        var prev = mainColumn.children[i].getPreviousPluginIndex(curr)

                        var prevPlugin
                        if (prev >= 0) {
                            prevPlugin = mainColumn.children[i].getPluginNameByIndex(prev)
                        } else {
                            var j = i - 1
                            if (j <= 0) {
                                j = mainColumn.children.length - 1 
                            }
                            prev = mainColumn.children[j].getPreviousPluginIndex(-1)
                            prevPlugin = mainColumn.children[j].getPluginNameByIndex(prev)
                        }

                        if (apl.columns > 1) {
                            loadPluginByName(prevPlugin)
                        } else {
                            main.currentPlugin = prevPlugin
                        }
                        break
                    }
                }
            }

            function selectNextPlugin() {
                for (var i = 0; i < mainColumn.children.length; ++i) {
                    var curr = mainColumn.children[i].getPluginIndexByName(main.currentPlugin)

                    if (curr >= 0) {
                        var next = mainColumn.children[i].getNextPluginIndex(curr)

                        var nextPlugin
                        if (next >= 0) {
                            nextPlugin = mainColumn.children[i].getPluginNameByIndex(next)
                        } else {
                            var j = i + 1
                            if (j >= mainColumn.children.length) {
                                j = 1 
                            }
                            next = mainColumn.children[j].getNextPluginIndex(-1)
                            nextPlugin = mainColumn.children[j].getPluginNameByIndex(next)
                        }
                        if (apl.columns > 1) {
                            loadPluginByName(nextPlugin)
                        } else {
                            main.currentPlugin = nextPlugin
                        }
                        break
                    }
                }
            }

            Keys.onPressed: {
                if (event.key == Qt.Key_Up) {
                    mainPage.selectPreviousPlugin()
                    event.accepted = true
                } else if (event.key == Qt.Key_Down) {
                    mainPage.selectNextPlugin()
                    event.accepted = true
                } else if (event.key == Qt.Key_Return || event.key == Qt.Key_Enter) {
                    if (apl.columns == 1 && main.currentPlugin) {
                        loadPluginByName(main.currentPlugin)
                        event.accepted = true
                    }
                }
            }

            PageHeader {
                id: standardHeader
                objectName: "standardHeader"
                visible: mainPage.header === standardHeader
                title: i18n.tr("System Settings")
                flickable: mainFlickable
                trailingActionBar.actions: [
                    Action {
                        objectName: "searchAction"
                        iconName: "find"
                        shortcut: StandardKey.Find
                        onTriggered: {
                            pluginManager.filter = "";
                            mainPage.header = searchHeader;
                        }
                    }
                ]
            }

            PageHeader {
                id: searchHeader
                objectName: "searchHeader"
                visible: mainPage.header === searchHeader
                flickable: mainFlickable
                contents: TextField {
                    id: searchField
                    objectName: "searchField"
                    anchors {
                        left: parent.left
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    inputMethodHints: Qt.ImhNoPredictiveText
                    onDisplayTextChanged: pluginManager.filter = displayText
                    placeholderText: i18n.tr("Search")
                    hasClearButton: false
                }
                onVisibleChanged: if (visible) searchField.forceActiveFocus()
                trailingActionBar.actions: [
                    Action {
                        objectName: "searchCancel"
                        iconName: "close"
                        shortcut: StandardKey.Cancel
                        onTriggered: {
                            pluginManager.filter = "";
                            searchField.text = "";
                            mainPage.header = standardHeader;
                        }
                    }
                ]
                z: 1
            }

            Flickable {
                id: mainFlickable
                anchors.fill: parent
                contentHeight: contentItem.childrenRect.height
                boundsBehavior: (contentHeight > mainPage.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
                /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
                   otherwise the UI might end up in a situation where scrolling doesn't work */
                flickableDirection: Flickable.VerticalFlick

                Column {
                    id: mainColumn
                    anchors.left: parent.left
                    anchors.right: parent.right

                    UncategorizedItemsView {
                        model: pluginManager.itemModel("uncategorized-top")
                    }

                    CategorySection {
                        category: "network"
                        categoryName: i18n.tr("Network")
                    }

                    CategorySection {
                        category: "personal"
                        categoryName: i18n.tr("Personal")
                    }

                    CategorySection {
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
