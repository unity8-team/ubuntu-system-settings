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
import Ubuntu.Connectivity 1.0
import Ubuntu.DownloadManager 1.2
import Ubuntu.SystemSettings.Update 1.0
import Ubuntu.Test 0.1

import Source 1.0

Item {
    id: testRoot
    width: units.gu(50)
    height: units.gu(90)

    Component {
        id: entryComponent

        EntryComponent {
            property var model
            property bool showAllUI: false
            width: testRoot.width
        }
    }

    // Test suite when there's no Image update.
    UbuntuTestCase {
        name: "EntryComponentNoImageUpdateTestCase"
        when: windowShown

        property var instance: null

        function init() {
            SystemImage.mockTargetBuildNumber(0);
            SystemImage.mockCurrentBuildNumber(0);
            instance = entryComponent.createObject(testRoot, {
                "model": {
                    displayName: "System Updates Test",
                    icon: "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"
                }
            });
        }

        function cleanup() {
            instance.destroy();
            UpdateManager.model.reset();
        }

        function test_defaultVisibility() {
            compare(instance.height, 0, "default visibility was wrong");
        }

        function test_clickUpdates() {
            UpdateManager.model.mockAddUpdate("app" + 0, 0, Update.KindClick);
            tryCompare(instance, "height", units.gu(6.5), 5000, "item did not have the correct height when we had click updates");
        }
    }

    // Test suite for when we have an image update.
    UbuntuTestCase {
        name: "EntryComponentImageUpdateTestCase"
        when: windowShown

        property var instance: null

        function init() {
            SystemImage.mockCurrentBuildNumber(0);
            SystemImage.mockTargetBuildNumber(1);
            instance = entryComponent.createObject(testRoot, {
                "model": {
                    displayName: "System Updates Test",
                    icon: "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"
                }
            });
        }

        function cleanup() {
            instance.destroy();
            UpdateManager.model.reset();
        }

        function test_visibleWhenImageUpdate() {
            tryCompare(instance, "height", units.gu(6.5), 5000, "item did not have the correct height when we had image updates");
        }

        function test_clickUpdates() {
            UpdateManager.model.mockAddUpdate("app" + 0, 0, Update.KindClick);
            tryCompare(instance, "height", units.gu(6.5), 5000, "item did not have the correct height when we had all types of updates");
        }
    }
}
