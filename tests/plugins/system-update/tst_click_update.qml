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
import Ubuntu.DownloadManager 1.2
import Ubuntu.SystemSettings.Update 1.0
import Ubuntu.Test 0.1

import Source 1.0

Item {
    id: testRoot
    width: units.gu(50)
    height: units.gu(90)

    Component {
        id: update

        ClickUpdate {
            width: testRoot.width
        }
    }

    function get_test_data() {
        return {
            updateState: SystemUpdate.StateAvailable,
            kind: SystemUpdate.KindApp,
            progress: 0,

            size: 1000,
            packageName: "some_app",
            revision: 1,
            command: "test click install --fake",
            name: "Some click app",
            clickToken: "hjiklmnopq",
            downloadUrl: "http://example.org/c.click",
            downloadSha512: "abcdefg",
            headers: {},
            remoteVersion: 2,
            changelog: "Changes",
            iconUrl: ""
        }
    }


    Component {
        id: cDownload

        SingleDownload {}
    }

    property var clickUpdateInstance: null

    SignalSpy {
        id: buttonSignalSpy
    }

    UbuntuTestCase {
        name: "ClickUpdateWithDownloadTest"
        when: windowShown

        function init() {
            clickUpdateInstance = update.createObject(testRoot, get_test_data());
            clickUpdateInstance.download = cDownload.createObject(clickUpdateInstance, {});
        }

        function cleanup() {
            clickUpdateInstance.download.destroy();
            clickUpdateInstance.destroy();
            clickUpdateInstance = null;
        }

        function test_onStarted() {
            clickUpdateInstance.download.mockStart();
            compare(clickUpdateInstance.updateState, SystemUpdate.StateQueuedForDownload);
            // Ensure indeterminate progress bar.
            compare(clickUpdateInstance.progress, -1);
        }

        function test_onProgress() {
            clickUpdateInstance.download.mockProgress(50);
            compare(clickUpdateInstance.progress, 50);
            compare(clickUpdateInstance.updateState, SystemUpdate.StateDownloading);
        }

        function test_onFinished() {
            clickUpdateInstance.download.mockFinished();
            compare(clickUpdateInstance.updateState, SystemUpdate.StateInstalled);
        }

        function test_onError() {
            clickUpdateInstance.download.mockErrorMessage("Error");
            compare(clickUpdateInstance.errorTitle, i18n.tr("Download failed"));
            compare(clickUpdateInstance.errorDetail, "Error");
            compare(clickUpdateInstance.updateState, SystemUpdate.StateFailed);
        }

        function test_onPause() {
            clickUpdateInstance.download.mockPause();
            compare(clickUpdateInstance.updateState, SystemUpdate.StateDownloadPaused);
        }

        function test_onResume() {
            clickUpdateInstance.download.mockResume();
            compare(clickUpdateInstance.updateState, SystemUpdate.StateDownloading);
        }

        function test_onProcessing() {
            clickUpdateInstance.download.mockProcess();
            compare(clickUpdateInstance.updateState, SystemUpdate.StateInstalling);
            // Ensure indeterminate progress bar.
            compare(clickUpdateInstance.progress, -1);
        }

        function test_startUpdate() {
            var button = findChild(clickUpdateInstance, "updateButton");
            clickUpdateInstance.updateState = SystemUpdate.StateAvailable;

            buttonSignalSpy.target = clickUpdateInstance;
            buttonSignalSpy.signalName = "install";
            mouseClick(button, button.width / 2, button.height / 2);
            buttonSignalSpy.wait();
        }

        function test_pauseUpdate() {
            var button = findChild(clickUpdateInstance, "updateButton");
            clickUpdateInstance.updateState = SystemUpdate.StateDownloading;

            buttonSignalSpy.target = clickUpdateInstance;
            buttonSignalSpy.signalName = "pause";
            mouseClick(button, button.width / 2, button.height / 2);
            buttonSignalSpy.wait();
        }

        function test_retryUpdate() {
            var button = findChild(clickUpdateInstance, "updateButton");
            clickUpdateInstance.updateState = SystemUpdate.StateFailed;

            buttonSignalSpy.target = clickUpdateInstance;
            buttonSignalSpy.signalName = "retry";
            mouseClick(button, button.width / 2, button.height / 2);
            buttonSignalSpy.wait();
        }

        function test_resumeUpdate() {
            var button = findChild(clickUpdateInstance, "updateButton");
            clickUpdateInstance.updateState = SystemUpdate.StateDownloadPaused;

            buttonSignalSpy.target = clickUpdateInstance;
            buttonSignalSpy.signalName = "resume";
            mouseClick(button, button.width / 2, button.height / 2);
            buttonSignalSpy.wait();
        }

        function test_invalidatedClickToken() {
            clickUpdateInstance.clickToken = "";
            compare(clickUpdateInstance.errorTitle, i18n.tr("Update failed"));
            compare(clickUpdateInstance.errorDetail, "The server responded incorrectly.");
            compare(clickUpdateInstance.updateState, SystemUpdate.StateFailed);
        }
    }

    UbuntuTestCase {
        name: "ClickUpdateWithoutDownloadTest"
        when: windowShown

        function init() {
            clickUpdateInstance = update.createObject(testRoot, get_test_data());

            var button = findChild(clickUpdateInstance, "updateButton");
            mouseClick(button, button.width / 2, button.height / 2);
        }

        function cleanup() {
            clickUpdateInstance.destroy();
            clickUpdateInstance = null;
        }

        function test_metadata() {
            var m = clickUpdateInstance.download.metadata;
            compare(m.command, get_test_data().command.split(" "));
            compare(m.title, get_test_data().name);
            compare(m.showInIndicator, false, "click update shown in indicator");
            compare(m.custom, {
                "packageName": get_test_data().packageName,
                "revision": get_test_data().revision
            });
        }

        function test_download() {
            var d = clickUpdateInstance.download;
            compare(d.url, get_test_data().downloadUrl);
            compare(d.autoStart, true, "click update is autostarting");
            compare(d.hash, get_test_data().downloadSha512);
            compare(d.algorithm, "sha512", "click update not using sha512 algo");
        }
    }
}



