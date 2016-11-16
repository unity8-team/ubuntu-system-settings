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
import Ubuntu.SystemSettings.Launcher 1.0

Item {
    id: testRoot
    width: 300
    height: 500

    property int largeScreen: units.gu(91)

    Component {
        id: pageComponent
        PageComponent {
            anchors.fill: parent
        }
    }

    UbuntuTestCase {
        name: "LauncherPageComponent"
        when: windowShown

        property var instance: null

        function init() {
            instance = pageComponent.createObject(testRoot, {});
        }

        function cleanup() {
            instance.destroy();
        }

        function get_gsettings_plugin() {
            return findInvisibleChild(instance, "unity8Settings");
        }

        function test_no_large_screen_label_necessary() {
            /* I.e. you're on a large screen, and the screen USS is rendered on
            is that screen. Assumes currentScreenNumber is 0. */
            var label = findChild(instance, "largeScreenLabel");
            LauncherPanelPlugin.setScreenGeometry(0, 0, 0, largeScreen, 100);
            LauncherPanelPlugin.setScreens(1);
            verify(!label.visible);
        }

        function test_large_screen_label_should_show() {
            /* I.e. you're on a small screen, but there's a large screen
            somewhere and USS is rendered onto that screen. */
            var label = findChild(instance, "largeScreenLabel");
            LauncherPanelPlugin.setScreenGeometry(0, 0, 0, 100, 100); // small
            LauncherPanelPlugin.setScreenGeometry(1, 0, 0, largeScreen, 100);
            LauncherPanelPlugin.setScreens(2);
            verify(label.visible);
        }

        function test_always_show_launcher_switch() {
            var gsettings = get_gsettings_plugin();
            var control = findChild(instance, 'alwaysShowLauncherSwitch');
            compare(control.checked, !gsettings.autohideLauncher);

            gsettings.autohideLauncher = !gsettings.autohideLauncher;
            compare(control.checked, !gsettings.autohideLauncher);

            control.trigger();
            compare(control.checked, !gsettings.autohideLauncher);
        }

        function test_icon_width_slider() {
            var gsettings = get_gsettings_plugin();
            var slider = findChild(instance, "iconWidth");
            compare(slider.value, gsettings.launcherWidth);

            gsettings.launcherWidth = 10;
            tryCompare(slider, "value", gsettings.launcherWidth);
        }
    }
}
