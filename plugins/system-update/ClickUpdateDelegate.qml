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

UpdateDelegate {
    id: update
    // property string packageName
    // property int revision
    // property var command
    // property string downloadSha512
    // property var udm: null
    // property var clickModel: null
    // property var downloadTracker: null

    updateState: Update.StateAvailable
    kind: Update.KindClick

    onInstall: download()
    onDownload: {
        createDownload()
    }
    // onRetry: retryUpdate(update.packageName)
    // onPause: downloadObject.pause()
    // onResume: downloadObject.resume()
    // onInstall: {
    //     console.warn('create download', model.title);
    //     if (model.downloadId) {
    //         console.warn('had download id', model.title, model.downloadId);
    //         return;
    //     }

    //     var metadata = {
    //         //"command": click.command,
    //         "title": model.title,
    //         "showInIndicator": false
    //     };
    //     var hdrs = {
    //         "X-Click-Token": model.token
    //     };
    //     var metadataObj = mdt.createObject(null, metadata);
    //     // metadataObj.custom = {
    //     //     "packageName": model.identifier,
    //     //     "revision": model.revision
    //     // };
    //     var singleDownloadObj = sdl.createObject(null, {
    //         // "url": click.downloadUrl,
    //         "autoStart": true,
    //         //"hash": click.downloadSha512,
    //         // "algorithm": "sha512",
    //         "headers": hdrs,
    //         "metadata": metadataObj
    //     });
    //     singleDownloadObj.download(model.downloadUrl);
    // }
    // onDownload: install()

    // signal retryUpdate(string name)

    states: [
        State {
            name: "failed"
            when: update.updateState === Update.StateFailed
            StateChangeScript {
                script: update.setError(
                    i18n.tr("Update failed"),
                    i18n.tr("Something went wrong.")
                )
            }
        }
    ]

    // Makes the progress bar indeterminate when waiting
    Binding {
        target: update
        property: "progress"
        value: -1
        when: {
            var queued = updateState === Update.StateQueuedForDownload;
            var installing = updateState === Update.StateInstalling;
            return queued || installing;
        }
    }

    function createDownload() {
        console.warn('createDownload', model, model.identifier, model.revision);

        if (model.downloadId) {
            console.warn('model already had downloadId', model.downloadId);
            return;
        }

        var metadata = {
            //"command": model.command,
            "title": model.title,
            "showInIndicator": false
        };
        var hdrs = {
            "X-Click-Token": model.token
        };
        var metadataObj = mdt.createObject(null, metadata);
        // metadataObj.custom = {
        //     "packageName": model.identifier,
        //     "revision": model.revision
        // };
        var singleDownloadObj = sdl.createObject(null, {
            // "url": model.downloadUrl,
            "autoStart": true,
            //"hash": model.downloadSha512,
            // "algorithm": "sha512",
            "headers": hdrs,
            "metadata": metadataObj,
            "revision": model.revision,
            "identifier": model.identifier
        });
        singleDownloadObj.download(model.downloadUrl);
        // bindDownload(singleDownloadObj);
    }

    // function pause(downloadId) {
    //     console.warn('pause', downloadId);
    //     // var dl = getDownload(downloadId);
    //     // if (dl) {
    //     //     console.warn('pause will pause download', dl);
    //     //     dl.pause();
    //     // } else {
    //     //     console.warn('pause found no download');
    //     // }
    // }

    Component {
        id: sdl

        SingleDownload {
            // property bool _bound: false

            property var identifier
            property var revision

            onDownloadIdChanged: {

                console.warn('downloid changed', downloadId);

                if (downloadId && identifier && (typeof revision !== "undefined")) {
                    // updateModel.setDownloadId(identifier, revision, downloadId);
                    // updateModel.startUpdate(downloadId);
                }
            }
            onProgressChanged: {
                console.warn('onProgressChanged', progress);

                // updateModel.setProgress(downloadId, progress);
            }
            onStarted: {
                console.warn('onStarted');

                // updateModel.startUpdate(downloadId);
            }
            onProcessing: {
                console.warn('onProcessing');

                // updateModel.processUpdate(downloadId);
            }
        }
    }

    Component {
        id: mdt
        Metadata {}
    }

}
