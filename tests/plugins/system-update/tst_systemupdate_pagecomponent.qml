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
        id: pageComponent

        PageComponent {
            title: "System Update Test"
            anchors.fill: parent
        }
    }

    UbuntuTestCase {
        name: "PageComponentTestCase"
        when: windowShown

        property var instance: null

        function init() {
            instance = pageComponent.createObject(testRoot, {});
        }

        function cleanup() {
            instance.destroy();
            SystemUpdate.model.reset();
        }

        function test_connectivity_data() {
            return [
                {
                    tag: "offline",
                    status: NetworkingStatus.Offline,
                },
                {
                    tag: "connecting",
                    status: NetworkingStatus.Connecting,
                },
                {
                    tag: "online",
                    status: NetworkingStatus.Online,
                }
            ]
        }

        function test_connectivity(data) {
            /* A prerequisite for connectivity to have an effect on the UI is
            that there are updates. */
            instance.updatesCount = 1;

            NetworkingStatus.setStatus(data.status);
            var overlay = findChild(instance, "overlay");
            compare(overlay.visible, data.status !== NetworkingStatus.Online);
        }

        function test_errors_data() {
            return [
                { status: SystemUpdate.StatusServerError },
                { status: SystemUpdate.StatusNetworkError }
            ];
        }

        function test_errors(data) {
            /* A prerequisite for errors to have an effect on the UI is
            that there are updates and we are online. */
            instance.online = true;
            instance.updatesCount = 1;

            instance.status = data.status;
            var overlay = findChild(instance, "overlay");
            compare(overlay.visible, true);
        }

        function test_auth_data() {
            return [
                { auth: false },
                { auth: true },
            ];
        }

        function test_auth(data) {
            /* A prerequisite for errors to have an effect on the UI is
            that we are online. */
            instance.online = true;

            instance.authenticated = data.auth;
            var notif = findChild(instance, "noAuthenticationNotification");
            compare(notif.visible, !data.auth);
        }

        function test_uoaConfic() {
            var notif = findChild(instance, "noAuthenticationNotification");
            var config = findInvisibleChild(instance, "uoaConfig");
            notif.requestAuthentication();

            compare(config.applicationId, "ubuntu-system-settings");
            compare(config.providerId, "ubuntuone");
            compare(config.execCalled(), true);
        }

        function test_clickUpdates_data() {
            return [
                { count: 0, visible: false },
                { count: 1, visible: true },
                { count: 50, visible: true },
            ];
        }

        function test_clickUpdates(data) {
            /* A prerequisite for errors to have an effect on the UI is
            that there are updates and we are online and authenticated. */
            instance.online = true;
            instance.authenticated = true;

            var clicks = findChild(instance, "updatesClickUpdates");
            for (var i = 0; i < data.count; i++) {
                SystemUpdate.model.mockAddUpdate("app" + i, i, Update.KindClick);
            }
            compare(clicks.visible, data.count > 0);
        }

        function test_imageUpdates_data() {
            return [
                { count: 0, visible: false },
                { count: 1, visible: true }
            ];
        }

        function test_imageUpdates(data) {
            /* A prerequisite for errors to have an effect on the UI is
            that there are updates and we are online. */
            instance.online = true;

            var images = findChild(instance, "updatesImageUpdate");
            for (var i = 0; i < data.count; i++) {
                SystemUpdate.model.mockAddUpdate("image" + i, i, Update.KindImage);
            }
            compare(images.visible, data.count > 0);
        }

        function test_previousUpdates_data() {
            return [
                { count: 0, visible: false },
                { count: 1, visible: true }
            ];
        }

        function test_previousUpdates(data) {
            // Note, no prerequisites.
            var previous = findChild(instance, "installedUpdates");
            for (var i = 0; i < data.count; i++) {
                SystemUpdate.model.mockAddUpdate("app" + i, i, Update.KindClick);
                SystemUpdate.model.setInstalled("app" + i, i);
            }
            compare(previous.visible, data.count > 0);
        }

        function test_lowPower() {
            instance.havePower = false;
            SystemUpdate.model.mockAddUpdate("ubuntu", 1, Update.KindImage);
            findChild(instance, "global").requestInstall();
            var dialog = findChild(testRoot, "imagePrompt");
            compare(findChild(dialog, "imagePromptInstall").visible, false);

            dialog.destroy();
            tryCompareFunction(function () {
                return !!findChild(testRoot, "imagePromptInstall");
            }, false);
        }

        function test_sufficientPower() {
            instance.havePower = true;
            SystemUpdate.model.mockAddUpdate("ubuntu", 1, Update.KindImage);
            findChild(instance, "global").requestInstall();
            var dialog = findChild(testRoot, "imagePrompt");
            compare(findChild(dialog, "imagePromptInstall").visible, true);

            dialog.destroy();
            tryCompareFunction(function () {
                return !!findChild(testRoot, "imagePromptInstall");
            }, false);
        }

        function test_imageActions() {

        }
    }


    UbuntuTestCase {
        name: "PageComponentBatchModeTestCase"
        when: windowShown

        property var instance: null

        function init() {
            instance = pageComponent.createObject(testRoot, {});

            // Requires that the user is authenticated, online and have power.
            instance.authenticated = true;
            instance.online = true;
            instance.havePower = true;

            // We always want some clicks for a batch mode
            SystemUpdate.model.mockAddUpdate("app" + 0, 0, Update.KindClick);
            SystemUpdate.model.mockAddUpdate("app" + 1, 1, Update.KindClick);
        }

        function cleanup() {
            instance.destroy();
            SystemUpdate.model.reset();
        }

        function test_clicksOnly() {
            findChild(instance, "global").install();
            compare(instance.batchMode, true);

            // Complete updates.
            SystemUpdate.model.setInstalled("app" + 0, 0);
            SystemUpdate.model.setInstalled("app" + 1, 1);

            tryCompare(instance, "batchMode", false);
        }

        function test_clicksAndImageAlreadyDownloaded() {
            SystemUpdate.model.mockAddUpdate("ubuntu", 1, Update.KindImage);
            wait(3000)
            findChild(instance, "global").requestInstall();

            tryCompareFunction(function () {
                return !!findChild(testRoot, "imagePrompt")
            }, true);

            var btn = findChild(testRoot, "imagePromptInstall");
            mouseClick(btn, btn.width / 2, btn.height / 2);

            tryCompareFunction(function () {
                return !!findChild(testRoot, "imagePrompt")
            }, false);
        }

        // Test when an image update is not even Downloaded.
        function test_clicksAndImageNotDownloaded() {
            SystemUpdate.model.mockAddUpdate("ubuntu", 1, Update.KindImage);
            findChild(instance, "global").requestInstall();

            tryCompareFunction(function () {
                return !!findChild(testRoot, "imagePrompt")
            }, true);

            var btn = findChild(testRoot, "imagePromptInstall");
            mouseClick(btn, btn.width / 2, btn.height / 2);

            tryCompareFunction(function () {
                return !!findChild(testRoot, "imagePrompt")
            }, false);
        }
    }

    function test_networkError() {

    }

    function test_serverError() {

    }
}
