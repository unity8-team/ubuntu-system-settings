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
import Ubuntu.SystemSettings.Update 1.0
import Ubuntu.Test 0.1

import Source 1.0

Item {
    id: testRoot
    width: units.gu(50)
    height: units.gu(90)

    Component {
        id: handler

        SystemImageHandler {
            width: testRoot.width
        }
    }

    Component {
        id: mockModel
        UpdateModel {}
    }

    Repeater {
        id: updateRepeater
        delegate: ImageUpdateDelegate {
            width: testRoot.width
            objectName: "image-update-" + index
            size: model.size
            version: model.remoteVersion
            updateState: model.updateState
            progress: model.progress
            errorDetail: error
        }
    }

    UbuntuTestCase {
        name: "SystemImageHandler"
        when: windowShown

        property var instance: null
        property var pendingModelInstance: null

        function init () {
            pendingModelInstance = mockModel.createObject(testRoot, {
                filter: UpdateModel.Pending
            });
            instance = handler.createObject(testRoot, {
                updateModel: pendingModelInstance
            });
            updateRepeater.model = pendingModelInstance;
        }

        function cleanup () {
            instance.destroy();
            pendingModelInstance.destroy();
        }

        function test_availableSignal_data() {
            return [
                {
                    tag: "normal signal, have update",
                    downloadMode: 0,
                    isAvailable: true, downloading: false,
                    availableVersion: "1", updateSize: 5000000,
                    lastUpdateDate: "", errorReason: "",
                    targetState: Update.StateAvailable
                },
                {
                    tag: "normal signal, no update",
                    downloadMode: 0,
                    isAvailable: false, downloading: false,
                    availableVersion: "", updateSize: 0,
                    lastUpdateDate: "", errorReason: ""
                },
                {
                    tag: "normal signal, update downloading",
                    downloadMode: 0,
                    isAvailable: true, downloading: true,
                    availableVersion: "1", updateSize: 0,
                    lastUpdateDate: "", errorReason: "",
                    targetState: Update.StateDownloading
                },
                {
                    tag: "normal signal, update paused",
                    downloadMode: 0,
                    isAvailable: true, downloading: false,
                    availableVersion: "1", updateSize: 0,
                    lastUpdateDate: "", errorReason: "paused",
                    targetState: Update.StateDownloadPaused
                },
                {
                    tag: "normal signal, update (automatically) downloading",
                    downloadMode: 1,
                    isAvailable: true, downloading: true,
                    availableVersion: "1", updateSize: 0,
                    lastUpdateDate: "", errorReason: "",
                    targetState: Update.StateDownloadingAutomatically
                },
                {
                    tag: "normal signal, update paused (automatically)",
                    downloadMode: 1,
                    isAvailable: true, downloading: false,
                    availableVersion: "1", updateSize: 0,
                    lastUpdateDate: "", errorReason: "paused",
                    targetState: Update.StateAutomaticDownloadPaused
                }
            ]
        }

        function test_availableSignal(data) {
            SystemImage.setDownloadMode(data.downloadMode);
            SystemImage.mockAvailableStatus(data.isAvailable, data.downloading, data.availableVersion,
                                            data.updateSize, data.lastUpdateDate, data.errorReason);
            tryCompare(pendingModelInstance, "count", data.isAvailable ? 1 : 0);
            var delegate = findChild(testRoot, "image-update-0");

            if (data.isAvailable) {
                compare(delegate.size, data.updateSize);
                compare(delegate.version, data.availableVersion);
            } else {
                verify(!delegate);
            }

            if (typeof data.targetState !== "undefined") {
                compare(delegate.updateState, data.targetState);
            }
        }

        function test_manuallyStarted () {
            SystemImage.setDownloadMode(0);
            SystemImage.mockTargetBuildNumber(300);
            pendingModelInstance.mockAddUpdate("ubuntu", 300, Update.KindImage);
            SystemImage.mockStarted();

            var delegate = findChild(testRoot, "image-update-0");
            compare(delegate.updateState, Update.StateDownloading);
        }

        function test_automaticallyStarted () {
            SystemImage.setDownloadMode(1);
            SystemImage.mockTargetBuildNumber(300);
            pendingModelInstance.mockAddUpdate("ubuntu", 300, Update.KindImage);
            SystemImage.mockStarted();
            var delegate = findChild(testRoot, "image-update-0");
            compare(delegate.updateState, Update.StateDownloadingAutomatically);
        }

        function test_automaticalProgress () {
            SystemImage.setDownloadMode(1);
            SystemImage.mockTargetBuildNumber(300);
            pendingModelInstance.mockAddUpdate("ubuntu", 300, Update.KindImage);
            SystemImage.mockProgress(50, 0); // pct, eta
            var delegate = findChild(testRoot, "image-update-0");
            var progressbar = findChild(delegate, "updateProgressbar");
            compare(delegate.updateState, Update.StateDownloadingAutomatically);
            compare(progressbar.visible, false);
            compare(progressbar.value, 50);
        }

        function test_manualProgress () {
            SystemImage.setDownloadMode(0);
            SystemImage.mockTargetBuildNumber(300);
            pendingModelInstance.mockAddUpdate("ubuntu", 300, Update.KindImage);
            SystemImage.mockProgress(50, 0); // pct, eta
            var delegate = findChild(testRoot, "image-update-0");
            var progressbar = findChild(delegate, "updateProgressbar");
            compare(delegate.updateState, Update.StateDownloading);
            compare(progressbar.visible, true);
            compare(progressbar.value, 50);
        }

        function test_automaticalPause () {
            SystemImage.setDownloadMode(1);
            SystemImage.mockTargetBuildNumber(300);
            pendingModelInstance.mockAddUpdate("ubuntu", 300, Update.KindImage);
            SystemImage.mockPaused(50);
            var delegate = findChild(testRoot, "image-update-0");
            compare(delegate.updateState, Update.StateAutomaticDownloadPaused);
        }

        function test_manualPause () {
            SystemImage.setDownloadMode(0);
            SystemImage.mockTargetBuildNumber(300);
            pendingModelInstance.mockAddUpdate("ubuntu", 300, Update.KindImage);
            SystemImage.mockPaused(50);
            var delegate = findChild(testRoot, "image-update-0");
            compare(delegate.updateState, Update.StateDownloadPaused);

        }

        function test_downloaded () {
            SystemImage.setDownloadMode(0);
            SystemImage.mockTargetBuildNumber(300);
            pendingModelInstance.mockAddUpdate("ubuntu", 300, Update.KindImage);
            SystemImage.mockDownloaded();
            var delegate = findChild(testRoot, "image-update-0");
            compare(delegate.updateState, Update.StateDownloaded);
        }

        function test_failed () {
            SystemImage.mockTargetBuildNumber(300);
            pendingModelInstance.mockAddUpdate("ubuntu", 300, Update.KindImage);
            SystemImage.mockFailed(3, "fail");
            wait(3000)
            var delegate = findChild(testRoot, "image-update-0");
            compare(delegate.updateState, Update.StateFailed);
            var error = findChild(delegate, "updateError");
            compare(error.visible, true);
            compare(error.title, i18n.tr("Update failed"));
            compare(error.detail, "fail");
        }

        function test_multiplePendingUpdates() {
            SystemImage.setDownloadMode(0);
            SystemImage.mockTargetBuildNumber(300);
            pendingModelInstance.mockAddUpdate("ubuntu", 300, Update.KindImage);
            SystemImage.mockAvailableStatus(true, false, 301,
                                            5000 * 1000, null, null);
            SystemImage.mockCurrentBuildNumber(300);
            compare(pendingModelInstance.count, 1);
        }
    }
}