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

    signal requestedRetry(string packageName, int revision)
    signal udmDownloadCreated(string packageName, int revision, int udmId)

    Component.onCompleted: {
        // console.warn("ClickUpdate", modelData, modelData.metadata, modelData.metadata.title, modelData.metadata.custom.iconUrl);
        // packageName = modelData.app_id
        // console.warn("have package name", packageName);
        if (udm_download_id) {
            download = Udm.getDownload(udm_download_id);
        } else {
                var metadata = {
                    "command": command.split(" "),
                    "title": title,
                    "showInIndicator": false,
                    // "downloadUrl": download_url,
                };
                console.warn("metadata", metadata);
                var headers = {
                    "X-Click-Token": click_token
                };
                console.warn("headers", metadata);
                var metadataObj = mdt.createObject(update, metadata);
                var singleDownloadObj = sdl.createObject(update, {
                    "url": download_url,
                    "autoStart": true,
                    "hash": download_sha512,
                    "algorithm": "sha512",
                    "headers": headers,
                    "metadata": metadataObj
                });
                singleDownloadObj.download(url);
                download = singleDownloadObj;
            }
        }
    }
    // Initial status, mode
    status: UpdateManager.NotStarted
    mode: UpdateManager.Installable

    name: title
    version: remote_version
    size: model.size
    iconUrl: icon_url
    changelog: changelog
    // progress: modelData.progress

    // If this failed, we tell our parents
    onRetry: requestedRetry(update.packageName, update.revision)

    // onDownload: modelData.start();
    // onPause: modelData.pause();
    // onInstall: modelData.start();

    Connections {
        target: download
        onErrorMessageChanged: {
            update.setError(
                i18n.tr("Download failed"), update.errorMessage
            )
            update.mode = UpdateManager.Retriable;
            update.status = UpdateManager.Failed;
        }
        onFinished: {
            console.warn('ClickUpdate Connections: Download finished')
            update.mode = UpdateManager.NonPausable;
            update.status = UpdateManager.Installed;
        }
        onProgressChanged: {
            console.warn('ClickUpdate Connections: Progress changed', progress)
            update.mode = UpdateManager.Pausable;
            update.status = UpdateManager.ManuallyDownloading;
        }
        onDownloadingChanged: {
            console.warn('ClickUpdate Connections: Downloading changed', download.downloading)
            if (download.downloading) {
                update.mode = UpdateManager.Pausable;
                update.status = UpdateManager.ManuallyDownloading;
            } else {
                update.mode = UpdateManager.Resumable;
                update.status = UpdateManager.DownloadPaused;
            }
        }
    }

    Component { id: sdl; SingleDownload { property string url; } }
    Component { id: mdt; Metadata {} }
}
