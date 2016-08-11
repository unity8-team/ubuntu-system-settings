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
        id: sdl
        SingleDownload {
            property bool _bound: false
        }
    }

    Component {
        id: mdt
        Metadata {}
    }

    Component {
        id: handler
        DownloadHandler {}
    }

    Repeater {
        id: updateRepeater
        model: UpdateManager.clickUpdates
        delegate: ClickUpdateDelegate {
            width: testRoot.width
            objectName: "click-update-" + index
            size: model.size
            version: model.remoteVersion
            updateState: model.updateState
            progress: model.progress
            error: model.error
        }
    }

    UbuntuTestCase {
        name: "DownloadHandlerTestWithExistingDownload"
        when: windowShown

        property var instance: null
        property var download: null
        property var metadata: null

        function init () {
            UpdateManager.model.mockAddUpdate("app", 1, Update.KindClick);
            metadata = mdt.createObject(testRoot, {
                custom: { identifier: "app", revision: 1, title: "test app"}
            });
            download = sdl.createObject(testRoot, {
                metadata: metadata,
            });
            instance = handler.createObject(testRoot, {
                updateModel: UpdateManager.model
            });
            instance.udm.mockDownload(download);
        }

        function cleanup () {
            metadata.destroy();
            download.destroy();
            instance.destroy();
            UpdateManager.model.reset();
        }

        function test_bindings() {
            verify(download._bound);
        }

        function test_progress() {
            download.mockProgress(50);
            tryCompare(findChild(testRoot, "click-update-0"), "progress", 50);
        }

        function test_finished() {
            instance.udm.mockDownloadFinished(download, "/test/path");
            verify(!findChild(testRoot, "click-update-0"), "delegate was not removed");
        }

        function test_paused() {
            instance.udm.mockDownloadPaused(download);
            compare(findChild(testRoot, "click-update-0").updateState, Update.StateDownloadPaused, "delegate was in wrong state");
        }

        function test_resumed() {
            instance.udm.mockDownloadResumed(download);
            compare(findChild(testRoot, "click-update-0").updateState, Update.StateDownloading, "delegate was in wrong state");
        }

        function test_canceled() {
            instance.udm.mockDownloadCanceled(download);
            compare(findChild(testRoot, "click-update-0").updateState, Update.StateAvailable, "delegate was in wrong state");
        }

        function test_error() {
            instance.udm.mockErrorFound(download, "failure");
            compare(findChild(testRoot, "click-update-0").updateState, Update.StateFailed, "delegate was in wrong state");
            compare(findChild(testRoot, "click-update-0").error, "failure", "delegate error value was wrong");
        }

        function test_pauseAndResume() {
            var click = {identifier: "app", revision: 1};

            download.start();

            instance.pauseDownload(click);
            verify(!download.downloading, "delegate was downloading when paused");

            instance.resumeDownload(click);
            verify(download.downloading, "delegate was not downloading");
        }

        function test_downloadExists() {
            var delegate = findChild(testRoot, "click-update-0");
            var origState = delegate.updateState;
            instance.assertDownloadExist({ identifier: "app", revision: 1});

            // It's state should not change.
            compare(delegate.updateState, origState, "delegate state was not reverted");
        }

        function test_getDownload() {
            var res = instance.getDownloadFor({ identifier: "app", revision: 1});
            compare(res, download, "got wrong download");
        }

        function test_getOnlyPertinentDownloads() {
            // We don't care about finished downloads.
            download.mockFinished();
            var res = instance.getDownloadFor({ identifier: "app", revision: 1});
            verify(!res, "got back download that was finished");
        }

        function test_duplicateDownloads() {
            var exists = { identifier: "app", revision: 1};
            compare(instance.createDownload(exists), null, "could created duplicate download");
        }

        function test_recreateFailedDownload() {
            var failed = { identifier: "app", revision: 1};
            instance.udm.mockErrorFound(download, "failure");
            verify(instance.createDownload(failed), "could not re-create failed download");
        }
    }

    UbuntuTestCase {
        name: "DownloadHandlerTestNoExistingDownloads"
        when: windowShown

        property var instance: null

        function init () {
            instance = handler.createObject(testRoot, {
                updateModel: UpdateManager.model
            });
        }

        function cleanup () {
            instance.destroy();
            UpdateManager.model.reset();
        }

        function test_createDownload() {
            var obj = instance.createDownload({
                command: "cmd",
                title: "title",
                token: "token",
                identifier: "app",
                revision: 1,
                downloadHash: "",
                downloadUrl: ""
            });
            compare(obj.metadata.custom.identifier, "app", "created download missing identifier")
            compare(obj.metadata.custom.revision, 1, "created download missing revision");
            compare(obj.headers["X-Click-Token"], "token", "created download missing token");
        }

        function test_downloadExists() {
            UpdateManager.model.mockAddUpdate("app", 1, Update.KindClick);
            var delegate = findChild(testRoot, "click-update-0");
            instance.assertDownloadExist({ identifier: "app", revision: 1});

            // An error will be displayed on the update.
            compare(delegate.updateState, Update.StateFailed, "delegate had wrong state");
            compare(delegate.error, i18n.tr("Installation failed"), "delegate had wrnog error value");
        }

        function test_retryDownload() {
            var obj = instance.retryDownload({
                command: "",
                title: "",
                token: "foo",
                identifier: "app",
                revision: 1,
                downloadHash: "",
                downloadUrl: ""
            });
            compare(obj.metadata.custom.identifier, "app", "download had wrong identifier")
            compare(obj.metadata.custom.revision, 1, "download had wrong revision");
            compare(obj.headers, {}, "download had headers, implying it was trying to do the X-Click-Token dance when it should not.");
        }
    }
}
