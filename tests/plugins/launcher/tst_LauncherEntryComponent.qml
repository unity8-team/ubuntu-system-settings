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
import Ubuntu.Test 0.1

import Source 1.0
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.Launcher 1.0

Item {
    id: testRoot
    width: 600
    height: units.gu(6)

    property int largeScreen: units.gu(91)

    Component {
        id: entryComponent
        EntryComponent {
            property var model
            property var pageStack
            anchors.fill: parent
        }
    }

    Item {
        id: fakePageStack
        property var currentPage: ({})
        signal pop()
    }

    SignalSpy {
        id: pageStackPopSpy
        target: fakePageStack
        signalName: "pop"
    }


    UbuntuTestCase {
        name: "LauncherEntryComponent"
        when: windowShown

        property var instance: null

        function init() {
            // Fake SystemSettings model
            var model = {
                item: {
                    baseName: "launcher",
                    translations: "ubuntu-system-settings"
                },
                icon: "settings-launcher.svg",
                displayName: "Launcher"
            };
            fakePageStack.currentPage = {
                objectName: "main"
            }
            instance = entryComponent.createObject(testRoot, {
                model: model,
                pageStack: fakePageStack
            });
        }

        function cleanup() {
            instance.destroy();
            pageStackPopSpy.clear();
        }

        function test_visible_if_large_screen() {
            LauncherPanelPlugin.setScreenGeometry(0, 0, 0, largeScreen, 100);
            LauncherPanelPlugin.setScreens(1);
            verify(instance.visible);
        }

        function test_invisible_if_small_screen() {
            LauncherPanelPlugin.setScreenGeometry(0, 0, 0, 100, 100);
            LauncherPanelPlugin.setScreens(1);
            verify(!instance.visible);
        }

        function test_pop_if_screen_unplugged_and_page_is_launcher() {
            LauncherPanelPlugin.setScreenGeometry(0, 0, 0, largeScreen, 100);
            LauncherPanelPlugin.setScreens(1);

            // Fake that the launcherPage is on the pageStack.
            fakePageStack.currentPage = {
                objectName: "launcherPage"
            }

            LauncherPanelPlugin.setScreens(0);
            pageStackPopSpy.wait();
            compare(pageStackPopSpy.count, 1);
        }

        function test_no_pop_if_page_is_not_launcher() {
            LauncherPanelPlugin.setScreenGeometry(0, 0, 0, largeScreen, 100);
            LauncherPanelPlugin.setScreens(1);

            fakePageStack.currentPage = {
                objectName: "someOtherPage"
            }

            LauncherPanelPlugin.setScreens(0);
            compare(pageStackPopSpy.count, 0);
        }
    }
}
