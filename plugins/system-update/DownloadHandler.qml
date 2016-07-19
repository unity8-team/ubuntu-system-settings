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
 *
 */
import QtQuick 2.4
import Ubuntu.DownloadManager 1.2

Item {
    id: root
    property var updateModel
    property alias downloads: udm.downloads

    DownloadManager {
        id: udm
        onDownloadFinished: {
            updateModel.setInstalled(download.metadata.custom.identifier,
                                     download.metadata.custom.revision);
        }
        onDownloadPaused: {
            updateModel.pauseUpdate(download.metadata.custom.identifier,
                                    download.metadata.custom.revision)
        }
        onDownloadResumed: {
            updateModel.resumeUpdate(download.metadata.custom.identifier,
                                     download.metadata.custom.revision)
        }
        onDownloadCanceled: {
            updateModel.cancelUpdate(download.metadata.custom.identifier,
                                     download.metadata.custom.revision)
        }
        onErrorFound: {
            updateModel.setError(download.metadata.custom.identifier,
                                 download.metadata.custom.revision,
                                 download.errorMessage)
        }
        onDownloadsChanged: restoreDownloads()
        Component.onCompleted: restoreDownloads()
    }

    function restoreDownloads() {
        var dl;
        for (var i = 0; i<downloads.length; i++) {
            dl = downloads[i];
            if (!dl._bound) {
                dl.progressChanged.connect(onDownloadProgress.bind(dl));
                dl._bound = true;
            }
        }
    }

    function resumeDownload(click) {
        var download = getDownloadFor(click);
        if (download && !download.downloading && !download.isCompleted) {
            download.resume();
        }
    }

    function pauseDownload(click) {
        var download = getDownloadFor(click);
        if (download && download.downloading) {
            download.pause();
        }
    }

    // Return download for a click update.
    function getDownloadFor(click) {
        var cust;
        var dl;
        for (var i = 0; i<downloads.length; i++) {
            dl = downloads[i];
            if (dl.errorMessage || dl.isCompleted) {
                // Ignore failed and completed downloads.
                continue;
            }

            cust = downloads[i].metadata.custom;

            if (cust.identifier === click.identifier && cust.revision === click.revision)
                return downloads[i];
        }
        return null;
    }

    function createDownload(click) {
        // Already had a download.
        var existingDownload = getDownloadFor(click);
        if (existingDownload !== null &&
            !existingDownload.errorMessage &&
            !existingDownload.isCompleted) {
            return;
        }

        var metadata = {
            "command": click.command,
            "title": click.title,
            "showInIndicator": false
        };
        var hdrs = {
            "X-Click-Token": click.token
        };
        var metadataObj = mdt.createObject(root, metadata);
        metadataObj.custom = {
            "identifier": click.identifier,
            "revision": click.revision
        };
        var singleDownloadObj = sdl.createObject(root, {
            // "url": click.downloadUrl,
            "autoStart": true,
            "hash": click.downloadHash,
            "algorithm": "sha512",
            "headers": hdrs,
            "metadata": metadataObj,
            "revision": click.revision,
            "identifier": click.identifier
        });
        singleDownloadObj.download(click.downloadUrl);
    }

    function onDownloadProgress (progress) {
        updateModel.setProgress(this.metadata.custom.identifier,
                                this.metadata.custom.revision,
                                this.progress);
    }

    function resumeAll() {
        var dl, id, rev;
        for (var i = 0; i < udm.downloads.length; i++) {
            dl = udm.downloads[i];
            id = dl.metadata.custom.identifier;
            rev = dl.metadata.custom.revision;
            if (id && (typeof rev !== "undefined")) {
                dl.resume();
                updateModel.resumeUpdate(id, rev);
            }
        }
    }

    function pauseAll() {
        var dl, id, rev;
        for (var i = 0; i < udm.downloads.length; i++) {
            dl = udm.downloads[i];
            id = dl.metadata.custom.identifier;
            rev = dl.metadata.custom.revision;
            if (id && (typeof rev !== "undefined")) {
                dl.pause();
                updateModel.pauseUpdate(id, rev);
            }
        }
    }

    /* If a update's model has a downloadId, check if UDM knows it. If not,
    treat this as a failure. Workaround for lp:1603770. */
    function assertDownloadExist(model) {
        if (!getDownloadFor(model)) {
            updateModel.setError(
                model.identifier, model.revision,
                i18n.tr("Download timed out. Please try again later.")
            );
        }
    }

    Component {
        id: sdl

        SingleDownload {
            property bool _bound: true

            onDownloadIdChanged: {
                updateModel.queueUpdate(metadata.custom.identifier,
                                        metadata.custom.revision,
                                        downloadId);
            }
            onProgressChanged: {
                updateModel.setProgress(metadata.custom.identifier,
                                        metadata.custom.revision,
                                        progress);
            }
            onStarted: {
                updateModel.startUpdate(metadata.custom.identifier,
                                        metadata.custom.revision);
            }
            onProcessing: {
                updateModel.processUpdate(metadata.custom.identifier,
                                          metadata.custom.revision);
            }
        }
    }

    Component {
        id: mdt
        Metadata {}
    }
}
