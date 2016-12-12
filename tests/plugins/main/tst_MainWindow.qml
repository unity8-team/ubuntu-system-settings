/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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
import QtTest 1.0
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Test 0.1

Item {
    id: testRoot
    width: units.gu(50)
    height: units.gu(90)

    Component {
        id: mainWindowComponent
        MainWindow {
            id: mainWindow
            anchors.fill: parent

            property string i18nDirectory: ""
            property string defaultPlugin: ""
            property var pluginOptions: ({})
            property var view: ({
                minimumWidth: 0,
                maximumWidth: 0,
            })
        }
    }

    UbuntuTestCase {
        name: "DefaultPluginMainWindowTest"
        when: windowShown

        function test_default_plugin () {
            mainWindowComponent.createObject(testRoot, {});
        }
    }

    UbuntuTestCase {
        name: "MainWindowTests"
        when: windowShown

        property var instance

        function init() {
            instance = mainWindowComponent.createObject(testRoot, {});
        }

        function cleanup() {
            instance.destroy();
        }

        function test_search_filter_results() {
            var standardHeader = findChild(instance, "standardHeader");
            var search = findChild(instance, "searchField");
            standardHeader.trailingActionBar.actions[0].trigger();
            search.text = "blue";
            compare(instance.pluginManager.filter, search.text)
        }

        function test_search_cleared_clears_filter() {
            var standardHeader = findChild(instance, "standardHeader");
            var searchHeader = findChild(instance, "searchHeader");
            var search = findChild(instance, "searchField");
            standardHeader.trailingActionBar.actions[0].trigger();
            search.text = "blue";
            searchHeader.trailingActionBar.actions[0].trigger();
            compare(instance.pluginManager.filter, "");
        }
    }

    UbuntuTestCase {
        name: "MainWindowAPLTests"
        when: windowShown

        Component {
            id: testPersonalEntry
            SettingsListItems.IconProgression {
                objectName: "testPersonalEntry"
                text: "Test"
                iconName: "system-users-symbolic"
            }
        }

        Component {
            id: testUncategorizedEntry
            SettingsListItems.IconProgression {
                objectName: "testUncategorizedEntry"
                text: "Test 2"
                iconName: "phone-smartphone-symbolic"
            }
        }

        Component {
            id: testPageComponent
            Page {
                objectName: "testPage"
                visible: false
                property var plugin
                property var pluginOptions
                property var pluginManager
                header: testHeader
                PageHeader {
                    id: testHeader
                    title: i18n.tr("Test PageComponent")
                    flickable: testFlickable
                }
                Flickable {
                    id: testFlickable
                    anchors.fill: parent
                    contentHeight: contentItem.childrenRect.height
                    Column {
                        anchors.left: parent.left
                        anchors.right: parent.right

                        Label {
                            text: "Test Content"
                        }
                    }
                }
            }
        }

        Component {
            id: pluginManagerComponent
            PluginManager {}
        }

        function waitForPage(page) {
            tryCompareFunction(function () {
                return !!findChild(testRoot, page);
            }, true);
            return findChild(testRoot, page);
        }

        function waitForPageDestruction(page) {
            tryCompareFunction(function () {
                return !!findChild(testRoot, page);
            }, false);
        }

        property var instance
        property var personalEntry
        property var uncategorizedEntry
        property var page
        property var manager

        function init() {
            manager = pluginManagerComponent.createObject(testRoot);
            personalEntry = testPersonalEntry;
            uncategorizedEntry = testUncategorizedEntry;
            page = testPageComponent;

            manager.addPlugin("Test", personalEntry, page, "personal");
            manager.addPlugin("Phone", uncategorizedEntry, page, "uncategorized-bottom");
        }

        function cleanup() {
            instance && instance.destroy();
        }

        function test_go_to_panel_data() {
            return [
                { tag: "one column, categorized", width: units.gu(50), entry: "testPersonalEntry" },
                { tag: "one column, uncategorized", width: units.gu(50), entry: "testUncategorizedEntry" },
                { tag: "two column, categorized", width: units.gu(110), entry: "testPersonalEntry" },
                { tag: "two column, uncategorized", width: units.gu(110), entry: "testUncategorizedEntry" },
            ];
        }
        function test_go_to_panel(data) {
            instance = mainWindowComponent.createObject(testRoot, {
                pluginManager: manager
            });
            waitForRendering(instance);
            testRoot.width = data.width;
            var entry = findChild(instance, data.entry);
            mouseClick(entry, entry.width / 2, entry.height / 2);
            waitForPage("testPage");
            var page = waitForPage("testPage");
            tryCompare(page, "visible", true);
        }

        function test_default_plugin_data() {
            return [
                { tag: "no default", default: "", pageObjectName: "" },
                { tag: "default", default: "Test", pageObjectName: "testPage" },
            ];
        }

        function test_default_plugin(data) {
            instance = mainWindowComponent.createObject(testRoot, {
                pluginManager: manager, defaultPlugin: data.default
            });
            if (data.pageObjectName) {
                var page = waitForPage(data.pageObjectName);
                tryCompare(page, "visible", true);
            }
        }

        function test_placeholder_plugin_created_and_destroyed() {
            instance = mainWindowComponent.createObject(testRoot, {
                pluginManager: manager, defaultPlugin: data.default,
                placeholderPlugin: "Test"
            });
            waitForPage("testPage");
            testRoot.width = units.gu(50);
            waitForPageDestruction("testPage");
        }

        function test_current_plugin_property() {
            var apl;
            var entry;

            instance = mainWindowComponent.createObject(testRoot, {
                pluginManager: manager
            });
            waitForRendering(instance);
            apl = findInvisibleChild(instance, "apl");

            // Base case
            compare(instance.currentPlugin, "");

            // Push a page
            instance.loadPluginByName("Test");
            tryCompare(instance, "currentPlugin", "Test");

            // Pop a page
            apl.removePages(apl.primaryPage);
            tryCompare(instance, "currentPlugin", "");
        }
    }
}
