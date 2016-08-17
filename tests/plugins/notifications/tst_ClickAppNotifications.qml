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
import Ubuntu.SystemSettings.Notifications 1.0 

import NotificationsSource 1.0

ClickAppNotifications {
    id: root

    width: 300
    height: 500

    TestCase {
        name: "ClickAppNotifications"
        when: windowShown

        /*
        function clickItem(item, button) {
            waitForRendering(item)
            if (button === undefined) button = Qt.LeftButton
            mouseClick(item, button)
            waitForRendering(item)
        }

        function init() {
            root.enableNotifications = true

            root.soundsNotify = true
            root.vibrationsNotify = true
            root.bubblesNotify = true
        }

        function test_verify_enabled_when_top_enabled() {
            var enableSwitch = findChild(root, "enableNotificationsSwitch")

            var soundsChecked = findChild(root, "soundsChecked")
            var vibrationsChecked = findChild(root, "vibrationsChecked")
            var bubblesChecked = findChild(root, "bubblesChecked")
            var listChecked = findChild(root, "listChecked")

            verify(enableSwitch.checked)

            verify(soundsChecked.enabled)
            verify(vibrationsChecked.enabled)
            verify(bubblesChecked.enabled)
            verify(listChecked.enabled)
        }

        function test_verify_disabled_when_top_disabled() {
            var enableSwitch = findChild(root, "enableNotificationsSwitch")

            var soundsChecked = findChild(root, "soundsChecked")
            var vibrationsChecked = findChild(root, "vibrationsChecked")
            var bubblesChecked = findChild(root, "bubblesChecked")
            var listChecked = findChild(root, "listChecked")

            verify(enableSwitch.checked)
            clickItem(enableSwitch)
            tryCompare(enableSwitch, "checked", false)

            verify(!soundsChecked.enabled)
            verify(!vibrationsChecked.enabled)
            verify(!bubblesChecked.enabled)
            verify(!listChecked.enabled)
        }
        */
    }
}
