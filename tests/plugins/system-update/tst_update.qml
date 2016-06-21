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

        Update {
            width: testRoot.width
        }
    }

    // We try here to address all the states which are enumerated in the
    // spec [1]. Due to limitations in DownloadManager, “Download” is
    // not possible on app updates. App updates are only Update-able
    // (i.e. Download+Install as one step).
    // [1] https://wiki.ubuntu.com/SoftwareUpdates#Phone
    UbuntuTestCase {
        name: "UpdateTest"
        when: windowShown

        SignalSpy {
            id: buttonSignalSpy
        }

        function cleanup() {
            buttonSignalSpy.target = null;
            buttonSignalSpy.signalName = "";
        }

        function test_update_data () {
            return [

                // Available
                {
                    tag: "System update available",

                    updateState: SystemUpdate.StateAvailable,
                    kind: SystemUpdate.KindSystem,
                    progress: 0,
                    button: { text: i18n.tr("Download"), visibility: true, state: true, signal: "download", },
                    progressbar: { visibility: false, progress: 0, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: "1 KB",
                    targetDownloadLabelText: "",
                },
                {
                    tag: "App update available",

                    updateState: SystemUpdate.StateAvailable,
                    kind: SystemUpdate.KindApp,
                    progress: 0,
                    button: { text: i18n.tr("Update"), visibility: true, state: true, signal: "install", },
                    progressbar: { visibility: false, progress: 0, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: "1 KB",
                    targetDownloadLabelText: "",
                },

                // Downloading automatically
                {
                    tag: "App update downloading automatically",

                    updateState: SystemUpdate.StateDownloadingAutomatically,
                    kind: SystemUpdate.KindApp,
                    progress: 0.5,
                    button: { text: i18n.tr("Pause"), visibility: true, state: true, signal: "pause", },
                    progressbar: { visibility: false, progress: 0.5, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("500 Bytes").arg("1 KB"),
                    targetDownloadLabelText: "",
                },
                {
                    tag: "System update downloading automatically",

                    updateState: SystemUpdate.StateDownloadingAutomatically,
                    kind: SystemUpdate.KindSystem,
                    progress: 0.5,
                    button: { text: i18n.tr("Pause"), visibility: true, state: true, signal: "pause", },
                    progressbar: { visibility: false, progress: 0.5, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("500 Bytes").arg("1 KB"),
                    targetDownloadLabelText: "",
                },

                // Downloading manually
                {
                    tag: "App update Downloading manually",

                    updateState: SystemUpdate.StateDownloading,
                    kind: SystemUpdate.KindApp,
                    progress: 0.5,
                    button: { text: i18n.tr("Pause"), visibility: true, state: true, signal: "pause", },
                    progressbar: { visibility: true, progress: 0.5, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("500 Bytes").arg("1 KB"),
                    targetDownloadLabelText: i18n.tr("Downloading"),
                },
                {
                    tag: "System update Downloading manually",

                    updateState: SystemUpdate.StateDownloading,
                    kind: SystemUpdate.KindSystem,
                    progress: 0.5,
                    button: { text: i18n.tr("Pause"), visibility: true, state: true, signal: "pause", },
                    progressbar: { visibility: true, progress: 0.5, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("500 Bytes").arg("1 KB"),
                    targetDownloadLabelText: i18n.tr("Downloading"),
                },

                // Download failed
                {
                    tag: "App update Download failed",

                    updateState: SystemUpdate.StateFailed,
                    kind: SystemUpdate.KindApp,
                    progress: 0.5,
                    button: { text: i18n.tr("Retry"), visibility: true, state: true, signal: "retry", },
                    progressbar: { visibility: false, progress: 0.5, },
                    error: { title: "Fail", detail: "Something failed big time.", visiblity: true, },
                    targetStatusLabelText: "",
                    targetDownloadLabelText: "",
                },
                {
                    tag: "System update Download failed",

                    updateState: SystemUpdate.StateFailed,
                    kind: SystemUpdate.KindSystem,
                    progress: 0.5,
                    button: { text: i18n.tr("Retry"), visibility: true, state: true, signal: "retry", },
                    progressbar: { visibility: false, progress: 0.5, },
                    error: { title: "Fail", detail: "Something failed big time.", visiblity: true, },
                    targetStatusLabelText: "",
                    targetDownloadLabelText: "",
                },

                // Downloaded
                {
                    tag: "App update Downloaded",
                    updateState: SystemUpdate.StateDownloaded,
                    kind: SystemUpdate.KindApp,
                    progress: 0.5,
                    button: { text: i18n.tr("Install"), visibility: true, state: true, signal: "install", },
                    progressbar: { visibility: false, progress: 0.5, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("Downloaded"),
                    targetDownloadLabelText: "",
                },
                {
                    tag: "System update Downloaded",

                    updateState: SystemUpdate.StateDownloaded,
                    kind: SystemUpdate.KindSystem,
                    progress: 0.5,
                    button: { text: i18n.tr("Install…"), visibility: true, state: true, signal: "install", },
                    progressbar: { visibility: false, progress: 0.5, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("Downloaded"),
                    targetDownloadLabelText: "",
                },

                // Waiting to download
                {
                    tag: "App update Waiting to download",
                    updateState: SystemUpdate.StateQueuedForDownload,
                    kind: SystemUpdate.KindApp,
                    progress: 0,
                    button: { text: i18n.tr("Pause"), visibility: true, state: true, signal: "pause", },
                    progressbar: { visibility: true, progress: 0, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: "",
                    targetDownloadLabelText: i18n.tr("Waiting to download"),
                },
                {
                    tag: "System update Waiting to download (Manual)",
                    updateState: SystemUpdate.StateQueuedForDownload,
                    kind: SystemUpdate.KindSystem,
                    progress: 0,
                    button: { text: i18n.tr("Pause"), visibility: true, state: true, signal: "pause", },
                    progressbar: { visibility: true, progress: 0, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: "",
                    targetDownloadLabelText: i18n.tr("Waiting to download"),
                },

                // Installing
                {
                    tag: "App update Installing",

                    updateState: SystemUpdate.StateInstalling,
                    kind: SystemUpdate.KindApp,
                    progress: 1,
                    button: { text: i18n.tr("Pause"), visibility: true, state: false, },
                    progressbar: { visibility: true, progress: 1, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: "",
                    targetDownloadLabelText: i18n.tr("Installing"),
                },
                {
                    tag: "System update Installing",

                    updateState: SystemUpdate.StateInstalling,
                    kind: SystemUpdate.KindSystem,
                    progress: 0.5,

                    button: { text: i18n.tr("Pause"), visibility: true, state: false, },
                    progressbar: { visibility: true, progress: 0.5, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: "",
                    targetDownloadLabelText: i18n.tr("Installing"),
                },

                // Download Paused
                {
                    tag: "App update Download Paused",

                    updateState: SystemUpdate.StateDownloadPaused,
                    kind: SystemUpdate.KindApp,
                    progress: 1,
                    button: { text: i18n.tr("Resume"), visibility: true, state: true, signal: "resume", },
                    progressbar: { visibility: true, progress: 1, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("1 KB").arg("1 KB"),
                    targetDownloadLabelText: i18n.tr("Paused"),
                },
                {
                    tag: "System update Download Paused (Manual)",

                    updateState: SystemUpdate.StateDownloadPaused,
                    kind: SystemUpdate.KindSystem,
                    progress: 0.5,
                    button: { text: i18n.tr("Resume"), visibility: true, state: true, signal: "resume", },
                    progressbar: { visibility: true, progress: 0.5, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("500 Bytes").arg("1 KB"),
                    targetDownloadLabelText: i18n.tr("Paused"),
                },
                {
                    tag: "System update Download Paused (Automatic)",
                    updateState: SystemUpdate.StateAutomaticDownloadPaused,
                    kind: SystemUpdate.KindSystem,
                    progress: 0.5,
                    button: { text: i18n.tr("Resume"), visibility: true, state: true, signal: "resume", },
                    progressbar: { visibility: false, progress: 0.5, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("%1 of %2").arg("500 Bytes").arg("1 KB"),
                    targetDownloadLabelText: "",
                },

                // Installed
                {
                    tag: "App update installed",
                    updateState: SystemUpdate.StateInstalled,
                    kind: SystemUpdate.KindApp,
                    progress: 1,
                    button: { text: i18n.tr("Pause"), visibility: true, state: false },
                    progressbar: { visibility: false, progress: 1, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("Installed"),
                    targetDownloadLabelText: "",
                },
                {
                    tag: "System update installed",
                    updateState: SystemUpdate.StateInstalled,
                    kind: SystemUpdate.KindSystem,
                    progress: 1,
                    button: { text: i18n.tr("Pause"), visibility: true, state: false },
                    progressbar: { visibility: false, progress: 1, },
                    error: { title: "", detail: "", visiblity: false, },
                    targetStatusLabelText: i18n.tr("Installed"),
                    targetDownloadLabelText: "",
                },
            ]
        }

        function test_update(data) {
            data["version"] = "0.42";
            data["changelog"] = "Changes";
            data["size"] = 1000;

            // Non-functional stuff
            if (data["kind"] === SystemUpdate.KindSystem) {
                data["name"] = "Ubuntu Touch";
                data["iconUrl"] = "distributor-logo.png";
            } else if (data["kind"] === SystemUpdate.KindApp) {
                data["name"] = "Some app";
                data["iconUrl"] = "system-settings.png";
            }

            var u = update.createObject(testRoot, data);
            var button = findChild(u, "updateButton");
            var statusLabel = findChild(u, "updateStatusLabel");
            var downloadLabel = findChild(u, "updateDownloadLabel");
            var progressbar = findChild(u, "updateProgressbar");
            var error = findChild(u, "updateError");

            compare(button.text, data.button.text, "button had wrong text");
            compare(button.visible, data.button.visibility, "button had wrong visibility");
            compare(button.enabled, data.button.state, "button had wrong enabled value");

            if (data.button.state === true) {
                buttonSignalSpy.target = u;
                buttonSignalSpy.signalName = data.button.signal;
                mouseClick(button, button.width / 2, button.height / 2);
                buttonSignalSpy.wait();
            }

            if (data.error.title && data.error.detail) {
                u.setError(data.error.title, data.error.detail);
            }

            compare(error.visible, data.error.visiblity, "error had wrong visibility");
            compare(error.title, data.error.title, "error title was wrong");
            compare(error.detail, data.error.detail, "error detail was wrong");

            compare(progressbar.visible, data.progressbar.visibility, "progress bar had wrong visibility");
            compare(progressbar.value, data.progressbar.progress, "progress bar had wrong value");

            compare(statusLabel.text, data.targetStatusLabelText, "status label had wrong text");

            compare(downloadLabel.text, data.targetDownloadLabelText, "download label had wrong text");

            u.destroy();
        }

        function test_retry() {

        }
    }
}
