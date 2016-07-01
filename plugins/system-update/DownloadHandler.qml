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
        cleanDownloads: false
        onDownloadFinished: {
            // console.warn('udm onDownloadFinished', download.downloadId);
            updateModel.setInstalled(download.metadata.custom.identifier,
                                     download.metadata.custom.revision);
        }

        onDownloadPaused: {
            // console.warn('udm onDownloadPaused', download.downloadId,
            //             download.metadata.custom.identifier, download.metadata.custom.revision);
            updateModel.pauseUpdate(download.metadata.custom.identifier,
                                    download.metadata.custom.revision)
        }

        onDownloadResumed: {
            // console.warn('udm onDownloadResumed', download.downloadId,
            //             download.metadata.custom.identifier, download.metadata.custom.revision);
            updateModel.resumeUpdate(download.metadata.custom.identifier,
                                     download.metadata.custom.revision)
        }

        onDownloadCanceled: {
            // console.warn('udm onDownloadCanceled', download.downloadId,
            //             download.metadata.custom.identifier, download.metadata.custom.revision);
            updateModel.cancelUpdate(download.metadata.custom.identifier,
                                     download.metadata.custom.revision)
        }

        onErrorFound: {
            // console.warn('udm onErrorFound', download.downloadId, download.errorMessage);
            updateModel.setError(download.metadata.custom.identifier,
                                 download.metadata.custom.revision,
                                 download.errorMessage)
        }
        onDownloadsChanged: restoreDownloads()
        Component.onCompleted: restoreDownloads()
    }

    function restoreDownloads() {
        for (var i = 0; i<downloads.length; i++) {
            // console.warn(downloads[i].downloadId, downloads[i].metadata.title);
            if (!downloads[i]._bound) {
                downloads[i].progressChanged.connect(onDownloadProgress.bind(downloads[i]));
                downloads[i]._bound = true;
            }
        }
    }

    function resumeDownload(click) {
        var download = getDownloadFor(click);
        if (download) {
            // console.warn('resuming', download.downloadId);
            download.resume();
        } else {
            // console.warn('resume failed');
        }
    }

    function pauseDownload(click) {
        var download = getDownloadFor(click);
        if (download) {
            // console.warn('pausing', download.downloadId);
            download.pause();
        } else {
            // console.warn('pause failed');
        }
    }

    function getDownloadFor(click) {
        var cust;
        for (var i = 0; i<downloads.length; i++) {
            cust = downloads[i].metadata.custom;
            if (cust.identifier === click.identifier && cust.revision === click.revision)
                return downloads[i];
        }
        return null;
    }

    function createDownload(click) {
        if (getDownloadFor(click) !== null) {
            // console.warn('createDownload, already have download for', click.identifier, click.revision);
            return;
        }

        // console.warn('createDownload', click, click.identifier, click.revision);

        var metadata = {
            //"command": click.command,
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
            //"hash": click.downloadSha512,
            // "algorithm": "sha512",
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

    Component {
        id: sdl

        SingleDownload {
            property bool _bound: true

            onDownloadIdChanged: {
                // console.warn('downloid changed', downloadId);
                updateModel.queueUpdate(metadata.custom.identifier,
                                      metadata.custom.revision);
            }
            onProgressChanged: {
                // console.warn('onProgressChanged', metadata.custom.identifier, metadata.custom.revision, progress);
                updateModel.setProgress(metadata.custom.identifier,
                                        metadata.custom.revision,
                                        progress);
            }
            onStarted: {
                // console.warn('onStarted', metadata.custom.identifier, metadata.custom.revision);

                updateModel.startUpdate(metadata.custom.identifier,
                                        metadata.custom.revision);
            }
            onProcessing: {
                // console.warn('onProcessing');
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

