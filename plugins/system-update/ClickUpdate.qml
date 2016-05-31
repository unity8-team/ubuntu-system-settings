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
    property var downloadId: null
    property var download: null

    onDownloadIdChanged: {
        if (downloadId === null) {
            var metadata = {
                "command": model.command.split(" "),
                "title": model.title,
                "showInIndicator": false,
                // "downloadUrl": download_url,
            };
            console.warn("metadata", metadata);
            var headers = {
                "X-Click-Token": model.click_token
            };
            console.warn("headers", metadata);
            var metadataObj = mdt.createObject(update, metadata);
            var singleDownloadObj = sdl.createObject(update, {
                "url": model.download_url,
                "autoStart": true,
                "hash": model.download_sha512,
                "algorithm": "sha512",
                "headers": model.headers,
                "metadata": metadataObj
            });
            singleDownloadObj.download(model.url);
            download = singleDownloadObj;
        }
    }

    updateState: UpdateManager.StateAvailable
    // Initial status, mode
    kind: UpdateManager.KindApp

    name: model.title
    version: model.remote_version
    size: model.size
    iconUrl: model.icon_url
    changelog: model.changelog
    // progress: model.progress


    onRetry: UpdateManager.retryClickPackage(update.packageName, update.revision)

    // onDownload: model.start();
    // onPause: model.pause();
    // onInstall: model.start();

    Connections {
        target: download
        onErrorChanged: {
            update.setError(
                i18n.tr("Download failed"), download.errorMessage
            )
            updateState = UpdateManager.StateFailed;
        }
        onFinished: {
            console.warn('ClickUpdate Connections: Download finished')
            updateState = UpdateManager.StateInstalled;
        }
        onProgressChanged: {
            console.warn('ClickUpdate Connections: Progress changed', download.progress)
            update.progress = download.progress;
            updateState = UpdateManager.StateDownloading;
        }
        onPaused: {
            updateState = UpdateManager.StateDownloadPaused;
        }
        onResumed: {
            updateState = UpdateManager.StateDownloading;
        }
    }

    Component { id: sdl; SingleDownload { property string url; } }
    Component { id: mdt; Metadata {} }
}
