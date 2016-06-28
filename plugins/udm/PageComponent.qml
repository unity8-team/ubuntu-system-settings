
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.DownloadManager 1.2
import SystemSettings 1.0

ItemPage {
    id: root
    function createDownload(title) {
        var metadataObj = mdt.createObject(root, {
            "showInIndicator": false,
            "title": title
        });
        var singleDownloadObj = sdl.createObject(root, {
            "autoStart": true,
            "metadata": metadataObj
        });
        singleDownloadObj.download("https://upload.wikimedia.org/wikiped" +
                                   "ia/commons/9/9a/Self-portrait_at_34_" +
                                   "by_Rembrandt.jpg");
        console.warn("created download", singleDownloadObj);
    }

    Column {
        width: parent.width

        Button {
            text: "start one"
            onClicked: createDownload("one")
        }

        Button {
            text: "start two"
            onClicked: createDownload("two")
        }
    }

    DownloadManager {
        id: udm
        onDownloadFinished: {
            console.warn('udm onDownloadFinished', download.downloadId);
        }
        onDownloadsChanged: {
            console.log('udm downloads changed..');
            for (var i = 0; i<downloads.length; i++) {
                console.log(i, 'changed');
                downloads[i].downloadId;
                downloads[i].metadata.title;
            }
        }
    }

    Component {
        id: sdl
        SingleDownload {

            onDownloadIdChanged: {
                console.warn("id changed for", metadata.title, downloadId)
            }
            onProgressChanged: {
                console.warn('Progress changed', progress, metadata.title);
            }
            onStarted: {
                console.warn('onStarted', metadata.title);
            }
        }
    }

    Component {
        id: mdt
        Metadata {}
    }
}
