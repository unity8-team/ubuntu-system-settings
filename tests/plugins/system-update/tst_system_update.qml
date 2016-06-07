/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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
        id: update

        SystemUpdate {
            backend: UpdateManager.systemImageBackend
            width: testRoot.width
        }
    }

    property var sUpdt: null

    SignalSpy {
        id: pauseSignalSpy
        signalName: "pause"
    }

    UbuntuTestCase {
        name: "ClickUpdateTest"
        when: windowShown

        function init () {
            sUpdt = update.createObject(testRoot, {});
        }

        function cleanup () {
            sUpdt.destroy();
            sUpdt = null;
        }

        function test_automaticallyStarted () {
            UpdateManager.systemImageBackend.downloadMode = 1;
            UpdateManager.systemImageBackend.mockStarted();
            compare(sUpdt.updateState, UpdateManager.StateDownloadingAutomatically);
        }

        function test_manuallyStarted () {
            UpdateManager.systemImageBackend.downloadMode = 0;
            UpdateManager.systemImageBackend.mockStarted();
            compare(sUpdt.updateState, UpdateManager.StateQueuedForDownload);
        }

        function test_automaticalProgress () {
            var progressbar = findChild(sUpdt, "updateProgressbar");
            UpdateManager.systemImageBackend.downloadMode = 1;
            UpdateManager.systemImageBackend.mockProgress(50, 0); // pct, eta
            compare(sUpdt.updateState, UpdateManager.StateDownloadingAutomatically);
            compare(progressbar.value, 50);
        }

        function test_manualProgress () {
            var progressbar = findChild(sUpdt, "updateProgressbar");
            UpdateManager.systemImageBackend.downloadMode = 0;
            UpdateManager.systemImageBackend.mockProgress(50, 0); // pct, eta
            compare(sUpdt.updateState, UpdateManager.StateDownloading);
            compare(progressbar.value, 50);
        }

        function test_automaticalPause () {
            var progressbar = findChild(sUpdt, "updateProgressbar");
            UpdateManager.systemImageBackend.downloadMode = 1;
            UpdateManager.systemImageBackend.mockPaused(50); // pct
            compare(sUpdt.updateState, UpdateManager.StateAutomaticDownloadPaused);
            compare(progressbar.value, 50);
        }

        function test_manualPause () {
            var progressbar = findChild(sUpdt, "updateProgressbar");
            UpdateManager.systemImageBackend.downloadMode = 0;
            UpdateManager.systemImageBackend.mockPaused(50); // pct
            compare(sUpdt.updateState, UpdateManager.StateDownloadPaused);
            compare(progressbar.value, 50);
        }

        function test_downloaded () {
            UpdateManager.systemImageBackend.mockDownloaded();
            compare(sUpdt.updateState, UpdateManager.StateDownloaded);
        }

        function test_failed () {
            var error = findChild(sUpdt, "updateError");
            UpdateManager.systemImageBackend.mockFailed(3, "fail");
            compare(sUpdt.updateState, UpdateManager.StateFailed);
            compare(error.visible, true);
            compare(error.title, i18n.tr("Update failed"));
            compare(error.detail, "fail");
        }

        function test_goingManualPausesDownload () {
            UpdateManager.systemImageBackend.downloadMode = 1;
            UpdateManager.systemImageBackend.mockProgress(50, 0); // pct, eta
            compare(sUpdt.updateState, UpdateManager.StateDownloadingAutomatically);

            pauseSignalSpy.target = sUpdt;

            // Set manual
            UpdateManager.systemImageBackend.downloadMode = 0;
            pauseSignalSpy.wait();
            UpdateManager.systemImageBackend.mockPaused(50); // pct
            compare(sUpdt.updateState, UpdateManager.StateDownloadPaused);
        }
    }
}
