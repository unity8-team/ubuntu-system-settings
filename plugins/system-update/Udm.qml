pragma Singleton

import QtQuick 2.4
import Ubuntu.DownloadManager 1.2

DownloadManager {
    id: udm
    property var clickPackageDownloads: {
        var dls = [];
        for (var i=0;i<udm.downloads.length; i++) {
            if (udm.downloads[i].metadata.custom.hasOwnProperty('package-name')) {
                dls.push(udm.downloads[i]);
            }
        }
        return dls;
    }

    function hasClickPackageDownload(packageName) {
        for (var i=0; i < clickPackageDownloads.length; i++) {
            var dl = clickPackageDownloads[i];
            if (dl.metadata.custom["package-name"] === packageName) {
                console.warn("hasClickPackageDownload finds", packageName, "in list of downloads.");
                return true;
            }
        }
        return false;
    }

    Component.onDestruction: {
        console.warn('UDM is being destroyed...');
        for (var i=0;i<clickPackageDownloads.length; i++) {
            var cpd = clickPackageDownloads[i];
            if (cpd.downloading || cpd.isCompleted || cpd.downloadInProgress) continue;
            clickPackageDownloads[i].cancel();
        }
        console.warn('Cancelled idle downloads.');
    }

    onDownloadFinished: console.warn('udm download finished', download, path);
    onErrorFound: console.warn('udm download error', download);
}
