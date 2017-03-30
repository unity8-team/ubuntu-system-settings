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
    property var pluginManager: PluginManager {}
    property string currentPlugin: ""
    property Page currentPluginPage: null

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
                currentPlugin = pluginName;
                currentPluginPage = page;
                page.Component.destruction.connect(function () {
                    if (mainPage) { 
                        mainPage.forceActiveFocus()
                    }
                }.bind(plugin))

                page.Keys.pressed.connect(function (event) {
                    if (event.key == Qt.Key_Left) {
                        if (apl.columns > 1) {
                            mainPage.activeFocusOnTab = true
                            mainPage.forceActiveFocus()
                            event.accepted = true
                        }
                    } else if (event.key == Qt.Key_Escape) {
                        if (apl.columns > 1) {
                            mainPage.activeFocusOnTab = true
                            mainPage.forceActiveFocus()
                        } else {
                            apl.removePages(apl.primaryPage);
                        }
                        event.accepted = true
                    }
                }.bind(plugin))
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
                if (currentPlugin == placeholderPlugin) {
                	apl.removePages(apl.primaryPage);
                } else {
                    main.currentPluginPage.forceActiveFocus()
                }
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

            PageHeader {
                id: standardHeader
                objectName: "standardHeader"
                // turn off automatic height
                // so when in APL the height doesn't change
                automaticHeight: false
                visible: mainPage.header === standardHeader
                title: i18n.tr("System Settings")
                flickable: categoriesListView
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
                // turn off automatic height
                // so when in APL the height doesn't change
                automaticHeight: false
                visible: mainPage.header === searchHeader
                flickable: categoriesListView
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

            ListView {
                id: categoriesListView
                anchors.fill: parent
                model: pluginManager.itemModel()
                focus: true
                section.property: "item.category"
                section.delegate: Loader {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    active: section !== "uncategorized-top" && section !== "uncategorized-bottom"
                    sourceComponent: SettingsItemTitle {
                        text: { 
                            if (section === "network") {
                                return i18n.tr("Network")
                            } else if (section === "personal") {
                                return i18n.tr("Personal")
                            } else if (section === "system") {
                                return i18n.tr("System")
                            }
                            return section
                        }
                    }
                }

                delegate: Loader {
                    id: loader
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
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
                    Binding {
                        target: loader.item
                        property: "activeFocusOnTab"
                        value: false
                    }
                }

                function selectPreviousPlugin() {
                    var idx = categoriesListView.model.getIndexByName(main.currentPlugin)
                    if (idx >= 0) {
                        var previous = categoriesListView.model.getPreviousVisibleIndex(idx)
                        while (previous == 0 || previous == 1) {
                            previous = categoriesListView.model.getPreviousVisibleIndex(previous)
                        }
                        if (apl.columns > 1) {
                            loadPluginByName(categoriesListView.model.getNameByIndex(previous))
                        } else {
                            main.currentPlugin = categoriesListView.model.getNameByIndex(previous)
                        }
                    }
                }

                function selectNextPlugin() {
                    var idx = categoriesListView.model.getIndexByName(main.currentPlugin)
                    if (idx >= 0) {
                        var next = categoriesListView.model.getNextVisibleIndex(idx)
                        while (next == 0 || next == 1) {
                            next = categoriesListView.model.getNextVisibleIndex(next)
                        }
                        if (apl.columns > 1) {
                            loadPluginByName(categoriesListView.model.getNameByIndex(next))
                        } else {
                            main.currentPlugin = categoriesListView.model.getNameByIndex(next)
                        }
                    }
                }

                Keys.onPressed: {
                    if (event.key == Qt.Key_Up) {
                        selectPreviousPlugin()
                        event.accepted = true
                    } else if (event.key == Qt.Key_Down) {
                        selectNextPlugin()
                        event.accepted = true
                    } else if (event.key == Qt.Key_Right) {
                        if (apl.columns > 1 && main.currentPluginPage) {
                            mainPage.activeFocusOnTab = false
                            main.currentPluginPage.forceActiveFocus()
                            event.accepted = true
                        }
                    } else if (event.key == Qt.Key_Return || event.key == Qt.Key_Enter) {
                        if (apl.columns == 1 && main.currentPlugin) {
                            loadPluginByName(main.currentPlugin)
                            mainPage.activeFocusOnTab = false
                            main.currentPluginPage.forceActiveFocus()
                            event.accepted = true
                        }
                    }
                }

                Connections {
                    target: main
                    onCurrentPluginChanged: {
                        var idx = categoriesListView.model.getIndexByName(main.currentPlugin)
                        categoriesListView.positionViewAtIndex(idx, ListView.End)
                    }
                }
            }
        }
    }
}
