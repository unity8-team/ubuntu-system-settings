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
 * This file represents the UI of the System Updates panel.
 */

import Ubuntu.SystemSettings.Update 1.0

DownloadManager {
    property var clickModel
    onDownloadFinished: {
        clickModel.setInstalled(download.downloadId)
    }

    onDownloadPaused: {
        clickModel.pauseDownload(download.downloadId)
    }

    onDownloadResumed: {
        clickModel.resumeDownload(download.downloadId)
    }

    onDownloadCanceled: {
        clickModel.cancelDownload(download.downloadId)
    }

    onErrorFound: {
        clickModel.setError(download.downloadId, download.errorMessage)
    }

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
}
