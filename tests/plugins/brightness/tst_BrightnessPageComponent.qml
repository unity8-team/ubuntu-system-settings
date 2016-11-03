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

        function test_no_mir_displays() {
            var repeater = findChild(instance, "displayConfigurationRepeater");
            compare(repeater.count, 0);
        }

        function test_one_mir_display() {
            var displayModel = get_panel_plugin().displayModel();
            var display = displayModel.mockAddDisplay();
            display.setConnected(true);
            var repeater = findChild(instance, "displayConfigurationRepeater");
            compare(repeater.count, 1);
        }
    }


    UbuntuTestCase {
        name: "BrightnessPageComponent for one Mir display"
        when: windowShown

        SignalSpy {
            id: signalSpy
            signalName: ""
            target: null
        }

        property var instance: null
        property var display: null

        function init() {
            instance = pageComponent.createObject(testRoot, {});
            var displayModel = get_panel_plugin().displayModel();
            display = displayModel.mockAddDisplay();
            display.setConnected(true);
            display.setName("test");
            display.save();
        }

        function cleanup() {
            instance.destroy();
            signalSpy.target = null;
        }

        function get_panel_plugin() {
            return findInvisibleChild(instance, "brightnessPanel");
        }

        function test_no_changes_disables_apply() {
            var obj = findChild(instance, "applyButton");
            verify(!obj.enabled);
        }

        function test_a_change_enables_apply() {
            var obj = findChild(instance, "enabledSwitch");
            var apply = findChild(instance, "applyButton");
            mouseClick(obj, obj.width / 2, obj.height / 2);
        }

        function test_apply() {
            signalSpy.target = get_panel_plugin();
            signalSpy.signalName = "applied";

            display.setEnabled(true);
            var obj = findChild(instance, "applyButton");
            mouseClick(obj, obj.width / 2, obj.height / 2);

            signalSpy.wait();
        }

        function test_default_rotation() {
            var obj = findChild(instance, "rotationSelector");
            compare(obj.selectedIndex, 0);
        }

        function test_other_rotation() {
            display.orientation = 1;
            var obj = findChild(instance, "rotationSelector");
            compare(obj.selectedIndex, 1);
        }

        function test_change_rotation() {
            var obj = findChild(instance, "rotationSelector");
            obj.delegateClicked(2);
            compare(display.orientation, 2);
        }

        function test_one_resolution() {
            display.mode = display.addMode(1024, 768, 60);
            var obj = findChild(instance, "resolutionLabel");
            var res = i18n.tr("%1×%2 @ %3hz").arg(1024).arg(768).arg(60);
            compare(obj.text, i18n.tr("Resolution: %1").arg(res));
        }

        function test_many_resolutions() {
            display.addMode(1024, 768, 60);
            display.addMode(640, 320, 40);
            display.mode = 1;
            var obj = findChild(instance, "resolutionSelector");
            compare(obj.selectedIndex, 1);
        }

        function test_change_resolution() {
            display.addMode(1024, 768, 60);
            display.addMode(640, 320, 40);
            display.mode = 1;
            var obj = findChild(instance, "resolutionSelector");
            obj.delegateClicked(0);
            compare(display.mode, 0);
        }
    }
}
