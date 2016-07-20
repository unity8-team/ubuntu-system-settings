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
        id: updates

        Updates {
            anchors.fill: parent
        }
    }

    Component {
        id: downloadHandlerComponent

        DownloadHandler {}
    }

    Component {
        id: imageHandlerComponent

        SystemImageHandler {}
    }

    Component {
        id: clickManagerComponent

        ClickManager {}
    }

    Component {
        id: mockModel

        UpdateModel {}
    }

    Component {
        id: mockImageModel

        UpdateModelFilter {
            kindFilter: Update.KindImage
            installed: false
        }
    }

    Component {
        id: mockClickModel

        UpdateModelFilter {
            kindFilter: Update.KindClick
            installed: false
        }
    }

    Component {
        id: mockInstalledModel

        UpdateModelFilter {
            installed: true
        }
    }

    SignalSpy {
        id: signalSpy
    }

    UbuntuTestCase {
        name: "UpdatesTestCase"
        when: windowShown

        property var instance: null
        property var model: null
        property var images: null
        property var clicks: null
        property var installed: null
        property var clickManager: null
        property var imageHandler: null
        property var downloadHandler: null

        function init() {
            model = mockModel.createObject(testRoot);

            images = mockImageModel.createObject(testRoot);
            clicks = mockClickModel.createObject(testRoot);
            installed = mockInstalledModel.createObject(testRoot);
            imageHandler = imageHandlerComponent.createObject(testRoot, {
                updateModel: model
            });
            downloadHandler = downloadHandlerComponent.createObject(testRoot, {
                updateModel: model
            });
            images.mockSetSourceModel(model);
            clicks.mockSetSourceModel(model);
            installed.mockSetSourceModel(model);

            clickManager = clickManagerComponent.createObject(testRoot);

            instance = updates.createObject(testRoot, {
                imageModel: images,
                clickModel: clicks,
                installedModel: installed,
                clickManager: clickManager,
                downloadHandler: downloadHandler,
                imageHandler: imageHandler
            });
        }

        function cleanup () {
            signalSpy.clear();
            signalSpy.target = null;
            signalSpy.signalName = "";
            instance.destroy();
            clickManager.destroy();

            images.destroy();
            clicks.destroy();
            installed.destroy();
            model.destroy();
            downloadHandler.destroy();
            imageHandler.destroy();
        }

        function test_connectivity_data() {
            return [
                { tag: "offline", online: false, targetState: "offline" },
                { tag: "online", online: true, targetState: "noUpdates" }
            ]
        }

        function test_connectivity(data) {
            instance.online = data.online;
            compare(instance.state, data.targetState);
        }

        function test_errors_data() {
            return [
                { status: SystemUpdate.StatusServerError, targetState: "error" },
                { status: SystemUpdate.StatusNetworkError, targetState: "error" }
            ];
        }

        function test_errors(data) {
            instance.online = true; // takes precedence over errors
            instance.status = data.status;
            compare(instance.state, data.targetState);
        }

        function test_auth_data() {
            return [
                { auth: false, targetState: "noAuth" },
                { auth: true, targetState: "noUpdates" },
            ];
        }

        function test_auth(data) {
            instance.online = true; // takes precedence over errors
            instance.authenticated = data.auth;
            compare(instance.state, data.targetState);
        }

        function test_signinPrompt_data() {
            return [
                { auth: false }, { auth: true }
            ]
        }

        function test_signinPrompt(data) {
            instance.online = true;
            instance.authenticated = data.auth;
            var prompt = findChild(instance, "updatesNoAuthNotif");

            // Visible when not authenticated.
            compare(prompt.visible, !data.auth);
        }

        function test_updatesCount_data() {
            return [
                { count: 0, targetState: "noUpdates" },
                { count: 1, targetState: "" },
            ];
        }

        function test_updatesCount(data) {
            instance.online = true; // takes precedence over errors
            instance.authenticated = true;
            instance.updatesCount = data.count;
            compare(instance.state, data.targetState);
        }

        function test_power_data() {
            return [
                { havePower: true },
                { havePower: false }
            ]
        }

        function test_power(data) {
            // Prerequisites:
            instance.online = true;
            instance.authenticated = true;
            instance.haveSystemUpdate = true;

            instance.havePower = data.havePower;

            // Add mock image update.
            model.mockAddUpdate("ubuntu", 300, Update.KindImage);
            model.setDownloaded("ubuntu", 300);

            // Trigger popup.
            var delegate = findChild(instance, "updatesImageDelegate-0");
            var btn = findChild(delegate, "updateButton");
            mouseClick(btn, btn.width / 2, btn.height / 2);

            var applyBtn = findChild(testRoot, "imagePromptInstall");
            var cancelBtn = findChild(testRoot, "imagePromptCancel");

            if (data.havePower) {
                compare(applyBtn.visible, true);
                signalSpy.signalName = "requestSystemUpdate";
                signalSpy.target = findChild(testRoot, "imagePrompt");
                mouseClick(applyBtn, applyBtn.width / 2, applyBtn.height / 2);
                signalSpy.wait();
            } else {
                compare(cancelBtn.visible, true);
                mouseClick(cancelBtn, cancelBtn.width / 2, cancelBtn.height / 2);
            }

            // Halt testing until dialog has been destroyed.
            tryCompareFunction(function() {
                return !!findChild(testRoot, "imagePrompt");
            }, false);
        }

        // Tests previous updates visibility in various modes.
        function test_previous_data() {
            return [
                { online: false, authenticated: false, targetVisiblity: true },
                { online: true, authenticated: false, targetVisiblity: true },
                { online: false, authenticated: true, targetVisiblity: true },
                { online: true, authenticated: true, targetVisiblity: true }
            ];
        }

        function test_previous(data) {
            instance.online = data.online;
            instance.authenticated = data.authenticated;

            // Add mock previous update.
            model.mockAddUpdate("app", 1, Update.KindImage);
            model.setInstalled("app", 1);

            compare(findChild(instance, "updatesInstalledUpdates").visible, data.targetVisiblity);
        }

        function test_installAllPrompts() {
            instance.online = true;
            instance.authenticated = true;
            instance.haveSystemUpdate = true;
            instance.havePower = true;

            model.mockAddUpdate("ubuntu", 300, Update.KindImage);
            model.mockAddUpdate("app", 1, Update.KindClick);

            var glob = findChild(instance, "updatesGlobal");
            glob.requestInstall();

            var btn = findChild(testRoot, "imagePromptInstall");
            mouseClick(btn, btn.width / 2, btn.height / 2);

            tryCompare(instance, "status", SystemUpdate.StatusBatchMode);
        }

        function test_installImagePrompts() {
            instance.online = true;
            instance.authenticated = true;
            instance.haveSystemUpdate = true;
            instance.havePower = true;

            model.mockAddUpdate("ubuntu", 300, Update.KindImage);
            model.setDownloaded("ubuntu", 300);

            var del = findChild(instance, "updatesImageDelegate-0");
            del.install();

            var btn = findChild(testRoot, "imagePromptInstall");
            mouseClick(btn, btn.width / 2, btn.height / 2);

            tryCompareFunction(function () {
                return SystemImage.isApplyRequested();
            }, true);
        }

        function test_integration_data() {
            return [
                { online: false, authenticated: false },
                { online: true, authenticated: false },
                { online: false, authenticated: true },
                { online: true, authenticated: true }
            ];
        }
        function test_integration(data) {
            instance.online = data.online;
            instance.authenticated = data.authenticated;
            instance.haveSystemUpdate = true;

            model.mockAddUpdate("ubuntu", 300, Update.KindImage);

            // Add mock previous updates.
            for (var i = 0; i < 5; i++) {
                model.mockAddUpdate("app-" + i, i, Update.KindClick);
                model.setInstalled("app-" + i, i);
            }

            // Add mock click updates.
            for (; i < 10; i++) {
                model.mockAddUpdate("app-" + i, i, Update.KindClick);
            }

            var glob = findChild(instance, "updatesGlobal");
            var overlay = findChild(instance, "updatesOverlay");
            var imageUpdate = findChild(instance, "updatesImageUpdate");
            var clickUpdates = findChild(instance, "updatesClickUpdates");
            var noAuthNotification = findChild(instance, "updatesNoAuthNotif");
            var installedUpdates = findChild(instance, "updatesInstalledUpdates");

            // Global is visible only when we're online and authed.
            compare(!glob.hidden, data.online && data.authenticated);

            // Overlay is visible only when not online
            compare(overlay.visible, !data.online);

            // As long as we're online, image updates are visible.
            compare(imageUpdate.visible, data.online);

            // If online and authenticated, we want click updates.
            compare(clickUpdates.visible, data.online && data.authenticated);

            // Not authenticated notification visible if online and not authed.
            compare(noAuthNotification.visible, data.online && !data.authenticated);

            // If there are installed updates, show them no matter what.
            compare(installedUpdates.visible, true);
        }
    }
}
