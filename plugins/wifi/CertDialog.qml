import QtQuick 2.0
import QtQuick.Layouts 1.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Wifi 1.0

Component {

    Dialog {
        id: certDialog

        property var certType;
        property var fileName;

        signal updateSignal(var update);

        anchors.fill: parent

        title: {
            if (certType === 0){ // certificate
                {i18n.tr("Add certificate")+"?"};
            } else if (certType === 1){ // privatekey
                {i18n.tr("Add key")+"?"};
            } else if (certType === 2){ // pacFile
                {i18n.tr("Add pac file")+"?"};
            }
        }

        FileHandler {
            id: fileHandler
        }

        Label {
            id: certContentLabel
            text : {i18n.tr("Content")+":"}
            objectName: "certContentLabel"
            fontSize: "medium"
            font.bold: false
        }

        TextArea {
            id : certContent
            objectName: "certContent"
            readOnly: true
            width: parent.width
            autoSize: true
            maximumLineCount: 7
            placeholderText: i18n.tr("No data available.")
            text: { fileHandler.getCertContent(certDialog.fileName).toString() }
        }

        RowLayout {
            id: buttonRow
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(2)
            height: cancelButton.height

            Button {
                id: cancelButton
                Layout.fillWidth: true
                text: i18n.tr("Cancel")
                onClicked: { fileHandler.removeFile(certDialog.fileName);
                    PopupUtils.close(certDialog);
                }
            }

            Button {
                id: saveButton
                text: i18n.tr("Save")
                Layout.fillWidth: true
                enabled: (certDialog.certContent.text !== "")
                onClicked: { if (certType === 0){ // certificate
                        fileHandler.moveCertFile(certDialog.fileName);
                    } else if (certType === 1){ // privatekey
                        fileHandler.moveKeyFile(certDialog.fileName);
                    } else if (certType === 2){ // pacFile
                        fileHandler.movePacFile(certDialog.fileName);
                    }

                    // just to be sure source file will be deleted if move was not successfull:
                    fileHandler.removeFile(certDialog.fileName);
                    certDialog.updateSignal(true);
                    PopupUtils.close(certDialog);
                }
            }
        }
    }
}
