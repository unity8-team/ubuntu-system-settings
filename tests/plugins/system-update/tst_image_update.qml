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
        id: update

        ImageUpdateDelegate {
            width: testRoot.width
        }
    }

    property var instance: null

    SignalSpy {
        id: pauseSignalSpy
        signalName: "pause"
    }

    UbuntuTestCase {
        name: "ClickUpdateTest"
        when: windowShown

        function init () {
            instance = update.createObject(testRoot, {});
        }

        function cleanup () {
            instance.destroy();
        }

        // function test_automaticallyStarted () {
        //     SystemImage.downloadMode = 1;
        //     SystemImage.mockStarted();
        //     compare(instance.updateState, Update.StateDownloadingAutomatically);
        // }

        // function test_manuallyStarted () {
        //     SystemImage.downloadMode = 0;
        //     SystemImage.mockStarted();
        //     compare(instance.updateState, Update.StateQueuedForDownload);
        // }

        // function test_automaticalProgress () {
        //     var progressbar = findChild(instance, "updateProgressbar");
        //     SystemImage.downloadMode = 1;
        //     SystemImage.mockProgress(50, 0); // pct, eta
        //     compare(instance.updateState, Update.StateDownloadingAutomatically);
        //     compare(progressbar.value, 50);
        // }

        // function test_manualProgress () {
        //     var progressbar = findChild(instance, "updateProgressbar");
        //     SystemImage.downloadMode = 0;
        //     SystemImage.mockProgress(50, 0); // pct, eta
        //     compare(instance.updateState, Update.StateDownloading);
        //     compare(progressbar.value, 50);
        // }

        // function test_automaticalPause () {
        //     var progressbar = findChild(instance, "updateProgressbar");
        //     SystemImage.downloadMode = 1;
        //     SystemImage.mockPaused(50); // pct
        //     compare(instance.updateState, Update.StateAutomaticDownloadPaused);
        //     compare(progressbar.value, 50);
        // }

        // function test_manualPause () {
        //     var progressbar = findChild(instance, "updateProgressbar");
        //     SystemImage.downloadMode = 0;
        //     SystemImage.mockPaused(50); // pct
        //     compare(instance.updateState, Update.StateDownloadPaused);
        //     compare(progressbar.value, 50);
        // }

        // function test_downloaded () {
        //     SystemImage.mockDownloaded();
        //     compare(instance.updateState, Update.StateDownloaded);
        // }

        // function test_failed () {
        //     var error = findChild(instance, "updateError");
        //     SystemImage.mockFailed(3, "fail");
        //     compare(instance.updateState, Update.StateFailed);
        //     compare(error.visible, true);
        //     compare(error.title, i18n.tr("Update failed"));
        //     compare(error.detail, "fail");
        // }

        // function test_goingManualPausesDownload () {
        //     SystemImage.downloadMode = 1;
        //     SystemImage.mockProgress(50, 0); // pct, eta
        //     compare(instance.updateState, Update.StateDownloadingAutomatically);

        //     pauseSignalSpy.target = instance;

        //     // Set manual
        //     SystemImage.downloadMode = 0;
        //     pauseSignalSpy.wait();
        //     SystemImage.mockPaused(50); // pct
        //     compare(instance.updateState, Update.StateDownloadPaused);
        // }
    }
}
