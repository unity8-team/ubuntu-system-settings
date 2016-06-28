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
import Ubuntu.SystemSettings.Update 1.0

Item {
    property var updateModel

    DownloadManager {
        id: udm

        onDownloadFinished: {
            console.warn('udm onDownloadFinished', download.downloadId);
            // updateModel.setInstalled(download.downloadId)
        }

        onDownloadPaused: {
            console.warn('udm onDownloadPaused', download.downloadId);
            // updateModel.pauseUpdate(download.downloadId)
        }

        onDownloadResumed: {
            console.warn('udm onDownloadResumed', download.downloadId);
            // updateModel.resumeUpdate(download.downloadId)
        }

        onDownloadCanceled: {
            console.warn('udm onDownloadCanceled', download.downloadId);
            // updateModel.cancelUpdate(download.downloadId)
        }

        onErrorFound: {
            console.warn('udm onErrorFound', download.downloadId, download.errorMessage);
            // updateModel.setError(download.downloadId, download.errorMessage)
        }
        onDownloadsChanged: {
            console.warn('udm udm downloads changed..');
            // for (var i = 0; i<downloads.length; i++) {
            //     console.warn(downloads[i].downloadId, downloads[i].metadata.title);
            // }
        }
    }

}

    // ListView {
    //     model: udm.downloads
    //     onCountChanged: console.warn('count changed in repeater', count)
    //     delegate: Rectangle {
    //         width: 100
    //         height: 10
    //         SingleDownload {
    //             id: sdl
    //             Component.onCompleted: console.warn('created a SingleDownload', sdl.downloadId)
    //             // property bool _bound: false
    //             onDownloadIdChanged: {
    //                 if (!metadata) {
    //                     console.warn('no metadata on', downloadId);
    //                     return;
    //                 }

    //                 if (!metadata.custom) {
    //                     console.warn('no custom metadata on', downloadId);
    //                     return;
    //                 }

    //                 var identifier = metadata.custom.packageName;
    //                 var revision = metadata.custom.revision;
    //                 console.warn('onDownloadIdChanged', identifier, revision, downloadId)

    //                 if (downloadId && identifier && (typeof revision !== "undefined")) {
    //                     clickUpdatesModel.setDownloadId(identifier, revision, downloadId);
    //                     clickUpdatesModel.startUpdate(downloadId);
    //                 }
    //             }
    //             onProgressChanged: {
    //                 console.warn('onProgressChanged', progress);
    //                 clickModel.setProgress(update.downloadId, progress);
    //             }
    //             onStarted: {
    //                 console.warn('onStarted');
    //                 clickModel.startUpdate(update.downloadId);
    //             }
    //             onProcessing: {
    //                 console.warn('onProcessing');
    //                 clickModel.processUpdate(update.downloadId);
    //             }
    //         }
    //     }
    // }


    // // onDownloadCanceled: SystemUpdate.udmDownloadEnded(download.downloadId) // (SingleDownload download)
    // onDownloadFinished: {
    //     console.warn('download FIN',
    //                  download,
    //                  download.metadata.title,
    //                  download.metadata.custom.packageName,
    //                  download.metadata.custom.revision);
    //     clickUpdateManager.markInstalled(
    //         download.metadata.custom.packageName, download.metadata.custom.revision
    //     );
    // }
    // onDownloadsChanged: console.warn('udm downloads changed', downloadManager.downloads);
    // // (SingleDownload download, QString path)
    // onErrorFound: {
    //     console.warn('error found!', download)
    // }
