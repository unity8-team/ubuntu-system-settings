pragma Singleton

import QtQuick 2.4
import Ubuntu.DownloadManager 1.2
import Ubuntu.SystemSettings.Update 1.0

DownloadManager {
    id: udm

    function getDownload(id) {
        for (var i=0; i < downloads.length; i++) {
            if (id === downloads[i].downloadId) return downloads[i];
        }
        return null;
    }

    onDownloadCanceled: UpdateManager.udmDownloadEnded(download.downloadId) // (SingleDownload download)
    onDownloadFinished: UpdateManager.udmDownloadEnded(download.downloadId) // (SingleDownload download, QString path)
    onErrorFound: UpdateManager.udmDownloadEnded(download.downloadId) // (SingleDownload download)
}
