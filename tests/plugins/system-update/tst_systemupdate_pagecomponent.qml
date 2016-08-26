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
            signal tornDown()
            Component.onDestruction: tornDown()
        }
    }

    SignalSpy {
        id: destroyedSpy
        signalName: "tornDown"
    }

    UbuntuTestCase {
        name: "PageComponentTestCase"
        when: windowShown

        property var instance: null

        function init() {
            instance = pageComponent.createObject(testRoot, {});
        }

        function cleanup() {
            destroyedSpy.target = instance;

            /* Not waiting here would cause a crash in some cases where
            UpdateManager.mockStatus would emit a signal to PageComponent which
            was in the process of being destroyed. Explicitly wait. */
            instance.destroy();
            destroyedSpy.wait();

            UpdateManager.mockStatus(UpdateManager.StatusIdle);
            UpdateManager.model.reset();
            SystemImage.reset();
        }

        function test_connectivity_data() {
            return [
                {
                    tag: "offline",
                    status: NetworkingStatus.Offline,
                    visible: true
                },
                {
                    tag: "connecting",
                    status: NetworkingStatus.Connecting,
                    visible: true
                },
                {
                    tag: "online",
                    status: NetworkingStatus.Online,
                    visible: false
                }
            ]
        }

        function test_connectivity(data) {
            instance.updatesCount = 1;

            NetworkingStatus.setStatus(data.status);
            var overlay = findChild(instance, "overlay");
            compare(overlay.visible, data.visible);
        }

        function test_errors_data() {
            return [
                { status: UpdateManager.StatusServerError },
                { status: UpdateManager.StatusNetworkError }
            ];
        }

        function test_errors(data) {
            instance.online = true;
            instance.updatesCount = 1;

            UpdateManager.mockStatus(data.status);
            var overlay = findChild(instance, "overlay");
            compare(overlay.visible, true);
        }

        function test_authenticationNotificationVisibility_data() {
            return [
                { auth: false, visible: true, status: UpdateManager.StatusIdle },
                { auth: false, visible: false, status: UpdateManager.StatusCheckingClickUpdates },
                { auth: false, visible: true, status: UpdateManager.StatusCheckingImageUpdates },
                { auth: false, visible: false, status: UpdateManager.StatusCheckingAllUpdates },
                { auth: false, visible: false, status: UpdateManager.StatusNetworkError },
                { auth: false, visible: false, status: UpdateManager.StatusServerError },
                { auth: true, visible: false, status: UpdateManager.StatusIdle },
                { auth: true, visible: false, status: UpdateManager.StatusCheckingClickUpdates },
                { auth: true, visible: false, status: UpdateManager.StatusCheckingImageUpdates },
                { auth: true, visible: false, status: UpdateManager.StatusCheckingAllUpdates },
                { auth: true, visible: false, status: UpdateManager.StatusNetworkError },
                { auth: true, visible: false, status: UpdateManager.StatusServerError }
            ];
        }

        function test_authenticationNotificationVisibility(data) {
            instance.online = true; // It's never shown if not online.
            UpdateManager.mockStatus(data.status);

            instance.authenticated = data.auth;
            var notif = findChild(instance, "noAuthenticationNotification");
            compare(notif.visible, data.visible);
        }

        function test_uoaConfic() {
            var notif = findChild(instance, "noAuthenticationNotification");
            var config = findInvisibleChild(instance, "uoaConfig");
            notif.requestAuthentication();

            compare(config.applicationId, "ubuntu-system-settings");
            compare(config.providerId, "ubuntuone");
            compare(config.execCalled(), true);
        }

        function test_clickUpdatesVisibility_data() {
            return [
                { count: 0, visible: false, status: UpdateManager.StatusIdle },
                { count: 0, visible: false, status: UpdateManager.StatusCheckingClickUpdates },
                { count: 1, visible: true, status: UpdateManager.StatusIdle },
                { count: 1, visible: false, status: UpdateManager.StatusCheckingClickUpdates },
                { count: 1, visible: true, status: UpdateManager.StatusCheckingImageUpdates },
                { count: 1, visible: false, status: UpdateManager.StatusCheckingAllUpdates },
                { count: 1, visible: false, status: UpdateManager.StatusNetworkError },
                { count: 1, visible: false, status: UpdateManager.StatusServerError }
            ];
        }

        function test_clickUpdatesVisibility(data) {
            instance.online = true;
            instance.authenticated = true;
            UpdateManager.mockStatus(data.status);

            var item = findChild(instance, "clickUpdates");
            for (var i = 0; i < data.count; i++) {
                UpdateManager.model.mockAddUpdate("app" + i, i, Update.KindClick);
            }
            compare(item.visible, data.visible);
        }

        function test_imageUpdatesVisibility_data() {
            return [
                { count: 0, visible: false, status: UpdateManager.StatusIdle },
                { count: 0, visible: false, status: UpdateManager.StatusCheckingImageUpdates },
                { count: 1, visible: true, status: UpdateManager.StatusIdle },
                { count: 1, visible: true, status: UpdateManager.StatusCheckingClickUpdates },
                { count: 1, visible: false, status: UpdateManager.StatusCheckingImageUpdates },
                { count: 1, visible: false, status: UpdateManager.StatusCheckingAllUpdates },
                { count: 1, visible: false, status: UpdateManager.StatusNetworkError },
                { count: 1, visible: false, status: UpdateManager.StatusServerError }
            ];
        }

        function test_imageUpdatesVisibility(data) {
            var item = findChild(instance, "imageUpdates");
            instance.online = true;
            UpdateManager.mockStatus(data.status);
            if (data.count) {
                UpdateManager.model.mockAddUpdate("ubuntu", 1, Update.KindImage);
            }
            compare(item.visible, data.visible);
        }

        function test_previousUpdatesVisibility_data() {
            return [
                { count: 0, visible: false },
                { count: 1, visible: true }
            ];
        }

        function test_previousUpdatesVisibility(data) {
            var previous = findChild(instance, "installedUpdates");
            for (var i = 0; i < data.count; i++) {
                UpdateManager.model.mockAddUpdate("app" + i, i, Update.KindClick);
                UpdateManager.model.setInstalled("app" + i, i);
            }
            compare(previous.visible, data.count > 0);
        }

        function test_noInstallOnLowPower() {
            instance.havePower = false;
            UpdateManager.model.mockAddUpdate("ubuntu", 1, Update.KindImage);
            findChild(instance, "global").requestInstall();
            var dialog = findChild(testRoot, "imagePrompt");
            compare(findChild(dialog, "imagePromptInstall").visible, false);

            dialog.destroy();
            tryCompareFunction(function () {
                return !!findChild(testRoot, "imagePromptInstall");
            }, false);
        }

        function test_installOnSufficientPower() {
            instance.havePower = true;
            UpdateManager.model.mockAddUpdate("ubuntu", 1, Update.KindImage);
            findChild(instance, "global").requestInstall();
            var dialog = findChild(testRoot, "imagePrompt");
            compare(findChild(dialog, "imagePromptInstall").visible, true);

            dialog.destroy();
            tryCompareFunction(function () {
                return !!findChild(testRoot, "imagePromptInstall");
            }, false);
        }

        function test_imageActions() {
            UpdateManager.model.mockAddUpdate("ubuntu", 1, Update.KindImage);
            var delegate = findChild(instance, "imageUpdatesDelegate-0");
            instance.havePower = true;
            SystemImage.downloadMode = 2; // Always download.

            delegate.retry();
            verify(SystemImage.called("downloadUpdate"));

            delegate.download();
            verify(SystemImage.called("downloadUpdate"));

            delegate.pause();
            verify(SystemImage.called("pauseDownload"));

            delegate.install();
            tryCompareFunction(function () {
                return !!findChild(testRoot, "imagePrompt")
            }, true);
            var dialog = findChild(testRoot, "imagePrompt");
            dialog.requestSystemUpdate();

            verify(SystemImage.called("applyUpdate"));

            dialog.destroy();
            tryCompareFunction(function () {
                return !!findChild(testRoot, "imagePromptInstall");
            }, false);
        }

        function test_forceAllowGSMDownload() {
            /* Test that we make s-i do what we want in cases where a Wi-Fi
            update has stalled. */
            SystemImage.downloadMode = 0; // Never download.
            UpdateManager.model.mockAddUpdate("ubuntu", 1, Update.KindImage);
            var delegate = findChild(instance, "imageUpdatesDelegate-0");
            delegate.download();
            verify(SystemImage.called("forceAllowGSMDownload"));
        }

        function test_forceAllowGSMDownloadRetry() {
            SystemImage.downloadMode = 0; // Never download.
            UpdateManager.model.mockAddUpdate("ubuntu", 1, Update.KindImage);
            var delegate = findChild(instance, "imageUpdatesDelegate-0");
            delegate.retry();
            verify(SystemImage.called("forceAllowGSMDownload"));
        }

        function test_imageUpdateFailureOverflow()
        {
            // Test that after N failures, we get an error message.
            SystemImage.mockUpdateFailed(4, "fail");
            tryCompareFunction(function () {
                return !!findChild(testRoot, "installationFailed")
            }, true);
            var dialog = findChild(testRoot, "installationFailed");
            dialog.destroy();
            tryCompareFunction(function () {
                return !!findChild(testRoot, "installationFailed");
            }, false);
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
            UpdateManager.model.mockAddUpdate("app" + 0, 0, Update.KindClick);
            UpdateManager.model.mockAddUpdate("app" + 1, 1, Update.KindClick);
        }

        function cleanup() {
            // See cleanup in topmost testcase for why we wait here.
            destroyedSpy.target = instance;
            instance.destroy();
            destroyedSpy.wait();

            UpdateManager.model.reset();
        }

        function test_clicksOnly() {
            findChild(instance, "global").install();
            compare(instance.batchMode, true);

            // Complete updates.
            UpdateManager.model.setInstalled("app" + 0, 0);
            UpdateManager.model.setInstalled("app" + 1, 1);

            tryCompare(instance, "batchMode", false);
        }

        function test_clicksAndImageAlreadyDownloaded() {
            UpdateManager.model.mockAddUpdate("ubuntu", 1, Update.KindImage);
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
            UpdateManager.model.mockAddUpdate("ubuntu", 1, Update.KindImage);
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
}
