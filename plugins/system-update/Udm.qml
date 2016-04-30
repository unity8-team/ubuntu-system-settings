pragma Singleton

import QtQuick 2.4
import Ubuntu.DownloadManager 1.2

DownloadManager {
    id: udm

    function getDownload(id) {
        for (var i=0; i < downloads.length; i++) {
            if (id === downloadId) return downloads[i];
        }
        return null;
    }
}
