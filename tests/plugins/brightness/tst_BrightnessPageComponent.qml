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
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.Brightness 1.0
import Ubuntu.Test 0.1

import Source 1.0

Item {
    id: testRoot
    width: 300
    height: 500

    Component {
        id: pageComponent
        PageComponent {
            anchors.fill: parent
        }
    }

    UbuntuTestCase {
        name: "BrightnessPageComponent"
        when: windowShown

        property var instance: null

        function init() {
            instance = pageComponent.createObject(testRoot, {});
        }

        function cleanup() {
            instance.destroy();
        }

        function get_panel_plugin() {
            return findInvisibleChild(instance, "brightnessPanel");
        }

        function get_aethercast_displays_plugin() {
            return findInvisibleChild(instance, "aethercastDisplays");
        }

        function test_brightness_title() {
            compare(instance.title, i18n.tr("Brightness"));
        }

        function test_brightness_and_displays_title() {
            get_panel_plugin().setWidiSupported(true);
            compare(instance.title, i18n.tr("Brightness & Display"));
        }

        function test_switch_disabled() {
            var swtch = findChild(instance, "externalDisplayControl");
            verify(!swtch.enabled);
        }

        function test_switch_enabled() {
            get_panel_plugin().setWidiSupported(true);
            var swtch = findChild(instance, "externalDisplayControl");
            verify(swtch.enabled);
        }

        function test_casting_enabled() {
            get_panel_plugin().setWidiSupported(true);
            get_aethercast_displays_plugin().setEnabled(true);
            var entry = findChild(instance, "displayCasting");

            verify(entry.enabled);
            compare(entry.value, i18n.tr("Not connected"));
        }
    }
}
