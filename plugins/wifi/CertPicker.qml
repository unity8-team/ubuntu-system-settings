import QtQuick 2.0
import QtQuick.Layouts 1.1
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Content 0.1

/* with a little help by looking into dekko mail client (lp:dekko) :*/
PopupBase  {
    //visible: false
    id: picker

    /* Picked Ffles will be stored in
        ~/.local/share/ubuntu-system-settings/Documents/

        At this state of implementation, this store will be used to parse the file to
        Networkmanager.
        As I haven't found that it is yet decided where to store cert files in general, all certs will be stored
        by parsing the cert content to NM. NM then creates a file (UUID-certType.pem) in /userdata/system-data/etc/NetworkManager/system-connections/ .
        At least a unique place.

        The files in ~/.local/share/ubuntu-system-settings/Documents/
        will not be deleted. Could be implemented later on.
        */


    signal fileImportSignal(var file)
    property var activeTransfer

    Rectangle {
        anchors.fill: parent

        ContentTransferHint {
            id: transferHint
            anchors.fill: parent
            activeTransfer: picker.activeTransfer
        }

        ContentStore {
            id: appStore
            scope: ContentScope.App
        }

        ContentPeerPicker {
            id: peerPicker
            anchors.fill: parent
            visible: true
            contentType: ContentType.Documents
            handler: ContentHandler.Source
            onPeerSelected: {
                peer.selectionType = ContentTransfer.Single;
                picker.activeTransfer = peer.request(appStore);
            }
            onCancelPressed: {
                PopupUtils.close(picker)
            }
        }
    }

    Connections {
        target: picker.activeTransfer ? picker.activeTransfer : null
        onStateChanged: {
            if (picker.activeTransfer.state === ContentTransfer.Charged) {
                if (picker.activeTransfer.items.length > 0) {
                    var fileUrl = picker.activeTransfer.items[0].url;
                    picker.fileImportSignal(fileUrl.toString().replace("file://", ""));
                    PopupUtils.close(picker);
                }

            } else if (picker.activeTransfer.state === ContentTransfer.Aborted){
                PopupUtils.close(picker);
            }
        }
    }

}
