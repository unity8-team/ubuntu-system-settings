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

        UpdateDelegate {
            width: testRoot.width
        }
    }

    /* We address all the states which are enumerated in the
     * spec, <https://wiki.ubuntu.com/SoftwareUpdates#Phone>.
     */
    UbuntuTestCase {
        name: "UpdateTest"
        when: windowShown

        property var instance: null

        SignalSpy {
            id: buttonSignalSpy
        }

        function cleanup() {
            instance && instance.destroy();
            buttonSignalSpy.target = null;
            buttonSignalSpy.signalName = "";
        }

        function test_update_data () {
            return [

                // Available
                {
                    tag: "System update available",

                    updateState: Update.StateAvailable,
                    kind: Update.KindImage,
                    progress: 0,
                    button: { text: i18n.tr("Download"), visibility: true, state: true, signal: "download", },
                    progressbar: { visibility: false, progress: 0, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: "1.0 kB",
                    targetDownloadLabelText: "",
                },
                {
                    tag: "Click update available",

                    updateState: Update.StateAvailable,
                    kind: Update.KindClick,
                    progress: 0,
                    button: { text: i18n.tr("Update"), visibility: true, state: true, signal: "install", },
                    progressbar: { visibility: false, progress: 0, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: "1.0 kB",
                    targetDownloadLabelText: "",
                },

                // Downloading automatically
                {
                    tag: "Click update downloading automatically",

                    updateState: Update.StateDownloadingAutomatically,
                    kind: Update.KindClick,
                    progress: 50,
                    button: { text: i18n.tr("Pause"), visibility: true, state: true, signal: "pause", },
                    progressbar: { visibility: false, progress: 50, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("500 bytes").arg("1.0 kB"),
                    targetDownloadLabelText: "",
                },
                {
                    tag: "System update downloading automatically",

                    updateState: Update.StateDownloadingAutomatically,
                    kind: Update.KindImage,
                    progress: 50,
                    button: { text: i18n.tr("Pause"), visibility: true, state: true, signal: "pause", },
                    progressbar: { visibility: false, progress: 50, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("500 bytes").arg("1.0 kB"),
                    targetDownloadLabelText: "",
                },

                // Downloading manually
                {
                    tag: "Click update Downloading manually",

                    updateState: Update.StateDownloading,
                    kind: Update.KindClick,
                    progress: 50,
                    button: { text: i18n.tr("Pause"), visibility: true, state: true, signal: "pause", },
                    progressbar: { visibility: true, progress: 50, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("500 bytes").arg("1.0 kB"),
                    targetDownloadLabelText: i18n.tr("Downloading"),
                },
                {
                    tag: "System update Downloading manually",

                    updateState: Update.StateDownloading,
                    kind: Update.KindImage,
                    progress: 50,
                    button: { text: i18n.tr("Pause"), visibility: true, state: true, signal: "pause", },
                    progressbar: { visibility: true, progress: 50, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("500 bytes").arg("1.0 kB"),
                    targetDownloadLabelText: i18n.tr("Downloading"),
                },

                // Download failed
                {
                    tag: "Click update Download failed",

                    updateState: Update.StateFailed,
                    kind: Update.KindClick,
                    progress: 50,
                    button: { text: i18n.tr("Retry"), visibility: true, state: true, signal: "retry", },
                    progressbar: { visibility: false, progress: 50, },
                    error: { text: "Something failed big time.", visiblity: true, },
                    targetStatusLabelText: "",
                    targetDownloadLabelText: "",
                },
                {
                    tag: "System update Download failed",

                    updateState: Update.StateFailed,
                    kind: Update.KindImage,
                    progress: 50,
                    button: { text: i18n.tr("Retry"), visibility: true, state: true, signal: "retry", },
                    progressbar: { visibility: false, progress: 50, },
                    error: { text: "Something failed big time.", visiblity: true, },
                    targetStatusLabelText: "",
                    targetDownloadLabelText: "",
                },

                // Downloaded
                {
                    tag: "Click update Downloaded",
                    updateState: Update.StateDownloaded,
                    kind: Update.KindClick,
                    progress: 50,
                    button: { text: i18n.tr("Install"), visibility: true, state: true, signal: "install", },
                    progressbar: { visibility: false, progress: 50, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("Downloaded"),
                    targetDownloadLabelText: "",
                },
                {
                    tag: "System update Downloaded",

                    updateState: Update.StateDownloaded,
                    kind: Update.KindImage,
                    progress: 50,
                    button: { text: i18n.tr("Install…"), visibility: true, state: true, signal: "install", },
                    progressbar: { visibility: false, progress: 50, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("Downloaded"),
                    targetDownloadLabelText: "",
                },

                // Waiting to download
                {
                    tag: "Click update Waiting to download",
                    updateState: Update.StateQueuedForDownload,
                    kind: Update.KindClick,
                    progress: 0,
                    button: { text: i18n.tr("Update"), visibility: true, state: false },
                    progressbar: { visibility: true, progress: 0, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: "",
                    targetDownloadLabelText: i18n.tr("Waiting to download"),
                },
                {
                    tag: "System update Waiting to download (Manual)",
                    updateState: Update.StateQueuedForDownload,
                    kind: Update.KindImage,
                    progress: 0,
                    button: { text: i18n.tr("Download"), visibility: true, state: false },
                    progressbar: { visibility: true, progress: 0, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: "",
                    targetDownloadLabelText: i18n.tr("Waiting to download"),
                },

                // Installing
                {
                    tag: "Click update Installing",

                    updateState: Update.StateInstalling,
                    kind: Update.KindClick,
                    progress: 1,
                    button: { text: i18n.tr("Pause"), visibility: true, state: false, },
                    progressbar: { visibility: true, progress: 1, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: "",
                    targetDownloadLabelText: i18n.tr("Installing"),
                },
                {
                    tag: "System update Installing",

                    updateState: Update.StateInstalling,
                    kind: Update.KindImage,
                    progress: 50,

                    button: { text: i18n.tr("Pause"), visibility: true, state: false, },
                    progressbar: { visibility: true, progress: 50, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: "",
                    targetDownloadLabelText: i18n.tr("Installing"),
                },

                // Download Paused
                {
                    tag: "Click update Download Paused",

                    updateState: Update.StateDownloadPaused,
                    kind: Update.KindClick,
                    progress: 100,
                    button: { text: i18n.tr("Resume"), visibility: true, state: true, signal: "resume", },
                    progressbar: { visibility: true, progress: 100, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("1.0 kB").arg("1.0 kB"),
                    targetDownloadLabelText: i18n.tr("Paused"),
                },
                {
                    tag: "System update Download Paused (Manual)",

                    updateState: Update.StateDownloadPaused,
                    kind: Update.KindImage,
                    progress: 50,
                    button: { text: i18n.tr("Resume"), visibility: true, state: true, signal: "resume", },
                    progressbar: { visibility: true, progress: 50, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("500 bytes").arg("1.0 kB"),
                    targetDownloadLabelText: i18n.tr("Paused"),
                },
                {
                    tag: "System update Download Paused (Automatic)",
                    updateState: Update.StateAutomaticDownloadPaused,
                    kind: Update.KindImage,
                    progress: 50,
                    button: { text: i18n.tr("Resume"), visibility: true, state: true, signal: "resume", },
                    progressbar: { visibility: false, progress: 50, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("500 bytes").arg("1.0 kB"),
                    targetDownloadLabelText: "",
                },

                // Install finished
                {
                    tag: "Click update install finished",
                    updateState: Update.StateInstallFinished,
                    kind: Update.KindClick,
                    progress: 1,
                    button: { text: i18n.tr("Pause"), visibility: true, state: false },
                    progressbar: { visibility: false, progress: 1, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("Installed"),
                    targetDownloadLabelText: "",
                },
                {
                    tag: "System update install finished",
                    updateState: Update.StateInstallFinished,
                    kind: Update.KindImage,
                    progress: 1,
                    button: { text: i18n.tr("Pause"), visibility: true, state: false },
                    progressbar: { visibility: false, progress: 1, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("Installed"),
                    targetDownloadLabelText: "",
                },
                {
                    tag: "System update installed",
                    updateState: Update.StateInstalled,
                    kind: Update.KindImage,
                    progress: 1,
                    button: { text: i18n.tr("Open"), visibility: false, state: false },
                    progressbar: { visibility: false, progress: 1, },
                    error: { text: "", visiblity: false, },
                    updatedAt: new Date("December 17, 1995 03:24:00"),
                    targetStatusLabelText: i18n.tr("Updated %1").arg(
                        new Date("December 17, 1995 03:24:00").toLocaleDateString(Qt.locale(), "d MMMM")
                    ),
                    targetDownloadLabelText: "",
                },

                // Crazy progress (beyond 100%)
                {
                    tag: "Crazy progress",
                    updateState: Update.StateDownloading,
                    kind: Update.KindClick,
                    progress: 150,
                    button: { text: i18n.tr("Pause"), visibility: true, state: true, signal: "pause" },
                    progressbar: { visibility: true, progress: 150, },
                    error: { text: "", visiblity: false, },
                    targetStatusLabelText: "1.0 kB",
                    targetDownloadLabelText: i18n.tr("Downloading"),
                },
            ]
        }

        function test_update(data) {
            data["version"] = "0.42";
            data["changelog"] = "Changes";
            data["size"] = 1000;

            // Non-functional stuff
            if (data["kind"] === Update.KindImage) {
                data["name"] = "Ubuntu Touch";
            } else if (data["kind"] === Update.KindClick) {
                data["name"] = "Some app";
            }

            instance = update.createObject(testRoot, data);
            var button = findChild(instance, "updateButton");
            var statusLabel = findChild(instance, "updateStatusLabel");
            var downloadLabel = findChild(instance, "updateDownloadLabel");
            var progressbar = findChild(instance, "updateProgressbar");
            var error = findChild(instance, "updateError");

            compare(button.text, data.button.text, "button had wrong text");
            compare(button.visible, data.button.visibility, "button had wrong visibility");
            compare(button.enabled, data.button.state, "button had wrong enabled value");

            if (data.button.state === true) {
                buttonSignalSpy.target = instance;
                buttonSignalSpy.signalName = data.button.signal;
                mouseClick(button, button.width / 2, button.height / 2);
                buttonSignalSpy.wait();
            }

            if (data.error.text) {
                instance.error = data.error.text;
            }

            compare(error.visible, data.error.visiblity, "error had wrong visibility");
            compare(instance.error, data.error.text, "error text was wrong");
            compare(progressbar.visible, data.progressbar.visibility, "progress bar had wrong visibility");
            compare(progressbar.value, data.progressbar.progress, "progress bar had wrong value");
            compare(statusLabel.text, data.targetStatusLabelText, "status label had wrong text");
            compare(downloadLabel.text, data.targetDownloadLabelText, "download label had wrong text");
        }
    }
}
