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
import Ubuntu.DownloadManager 1.2
import Ubuntu.SystemSettings.Update 1.0

Update {
    id: update
    property string packageName
    property int revision
    property var download: null
    property var clickUpdateManager: ClickUpdateManager {}
    property string command
    property string clickToken
    property string downloadUrl
    property string downloadSha512
    property string headers

    updateState: SystemUpdate.StateAvailable
    kind: SystemUpdate.KindApp

    onRetry: clickUpdateManager.check(update.packageName)
    onPause: download.pause()
    onResume: download.resume()
    onInstall: {
        if (download === null) {
            var metadata = {
                "command": update.command.split(" "),
                "title": update.name,
                "showInIndicator": false,
                "custom": {
                    "packageName": update.packageName,
                    "revision": update.revision
                }
            };
            var hdrs = {
                "X-Click-Token": update.clickToken
            };
            var metadataObj = mdt.createObject(update, metadata);
            var singleDownloadObj = sdl.createObject(update, {
                "url": update.downloadUrl,
                "autoStart": false,
                "hash": update.downloadSha512,
                "algorithm": "sha512",
                "headers": hdrs,
                "metadata": metadataObj
            });
            singleDownloadObj.download(update.downloadUrl);
            download = singleDownloadObj;
        }

        download.download(update.downloadUrl)
    }
    onDownload: install()

    states: [
        State {
            name: "invalidClickToken"
            when: update.clickToken === ""
            StateChangeScript {
                script: update.setError(
                    i18n.tr("Update failed"),
                    i18n.tr("The server responded incorrectly.")
                )
            }
            PropertyChanges {
                target: update
                updateState: SystemUpdate.StateFailed
            }
        }
    ]

    Connections {
        target: download
        onErrorChanged: {
            update.setError(
                i18n.tr("Download failed"), download.errorMessage
            )
            updateState = SystemUpdate.StateFailed;
        }
        onFinished: {
            updateState = SystemUpdate.StateInstalled;
        }
        onProgressChanged: {
            update.progress = download.progress;
            updateState = SystemUpdate.StateDownloading;
        }
        onPaused: {
            updateState = SystemUpdate.StateDownloadPaused;
        }
        onResumed: {
            updateState = SystemUpdate.StateDownloading;
        }
        onStarted: {
            updateState = SystemUpdate.StateQueuedForDownload;
        }
        onProcessing: {
            updateState = SystemUpdate.StateInstalling;
        }
    }

    Component { id: sdl; SingleDownload { property string url; } }
    Component { id: mdt; Metadata {} }

    // Makes the progress bar indeterminate when waiting
    Binding {
        target: update
        property: "progress"
        value: -1
        when: {
            var queued = updateState === SystemUpdate.StateQueuedForDownload;
            var installing = updateState === SystemUpdate.StateInstalling;
            return queued || installing;
        }
    }
}
