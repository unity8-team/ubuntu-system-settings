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
            property var view: ({
                minimumWidth: 0,
                maximumWidth: 0,
            })
        }
    }

    // UbuntuTestCase {
    //     name: "MainWindowTests"
    //     when: windowShown

    //     property var instance

    //     function init() {
    //         instance = mainWindowComponent.createObject(testRoot, {});
    //     }

    //     function cleanup() {
    //         instance.destroy();
    //     }

    //     function test_search_filter_results() {
    //         var standardHeader = findChild(instance, "standardHeader");
    //         var search = findChild(instance, "searchField");
    //         standardHeader.trailingActionBar.actions[0].trigger();
    //         search.text = "blue";
    //         compare(instance.pluginManager.filter, search.text)
    //     }

    //     function test_search_cleared_clears_filter() {
    //         var standardHeader = findChild(instance, "standardHeader");
    //         var searchHeader = findChild(instance, "searchHeader");
    //         var search = findChild(instance, "searchField");
    //         standardHeader.trailingActionBar.actions[0].trigger();
    //         search.text = "blue";
    //         searchHeader.trailingActionBar.actions[0].trigger();
    //         compare(instance.pluginManager.filter, "");
    //     }
    // }


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
            id: testNetworkEntryComponent
            SettingsListItems.IconProgression {
                objectName: "testNetworkEntryComponent"
                text: "Test 2"
                iconName: "phone-smartphone-symbolic"
            }
        }

        Component {
            id: testPageComponent
            Page {
                visible: false
                property var pluginManager
                header: testHeader
                PageHeader {
                    id: testHeader
                    title: i18n.tr("Test PageComponent")
                    flickable: testFlickable
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
        }

        Component {
            id: pluginManagerComponent
            PluginManager {}
        }

        property var instance
        property var personalEntry
        property var networkEntry
        property var page
        property var manager

        function init() {
            manager = pluginManagerComponent.createObject(testRoot);

            // entry = testEntryComponent.createObject(testRoot);
            // page = testPageComponent.createObject(testRoot);
            personalEntry = testPersonalEntry;
            networkEntry = testNetworkEntryComponent;
            page = testPageComponent;

            manager.addPlugin('Test', personalEntry, page, "personal");
            manager.addPlugin('Phone', networkEntry, page, "network");
            instance = mainWindowComponent.createObject(testRoot, {
                pluginManager: manager
            });
        }

        function cleanup() {
            instance.destroy();
        }

        function test_wait(){
            wait(2000)
            testRoot.width = units.gu(50)
            wait(2000)
            testRoot.width = units.gu(150)
            wait(2000)
            testRoot.width = units.gu(90)
            wait(2000)
            testRoot.width = units.gu(110)
            wait(2000)
        }
    }
}
