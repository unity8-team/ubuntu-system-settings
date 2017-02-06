import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.SystemSettings.Wifi 1.0

Component {

    Dialog {
        id: certDialog

        property var certType;
        property var fileName;

        signal updateSignal(var update);
        signal certSaved(string file);

        anchors.fill: parent

        title: {
            if (certType === 0) { // certificate
                return i18n.tr("Add certificate?");
            } else if (certType === 1) { // privatekey
                return i18n.tr("Add key?");
            } else if (certType === 2) { // pacFile
                return i18n.tr("Add pac file?");
            }
        }

        Component {
            id: failedToImportComponent
            Dialog {
                id: failedToImportDialog
                title: {
                    if (certType === 0) { // certificate
                        return i18n.tr("Could not save certificate.")
                    } else if (certType === 1) { // privatekey
                        return i18n.tr("Could not save key.")
                    } else if (certType === 2) { // pacFile
                        return i18n.tr("Could not save pack file.")
                    }
                }

                Button {
                    text: i18n.tr("OK")
                    onClicked: {
                        PopupUtils.close(failedToImportDialog);
                        PopupUtils.close(certDialog);
                    }
                }
            }
        }

        FileHandler {
            id: fileHandler
        }

        Label {
            id: certContentLabel
            text : i18n.tr("Content:")
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
            text: fileHandler.getCertContent(certDialog.fileName).toString()
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
                onClicked: {
                    PopupUtils.close(certDialog);
                }
            }

            Button {
                id: saveButton
                text: i18n.tr("Save")
                Layout.fillWidth: true
                enabled: (certDialog.certContent.text !== "")
                onClicked: {
                    var ret;
                    if (certType === 0) { // certificate
                        ret = fileHandler.copyCertFile(certDialog.fileName);
                    } else if (certType === 1) { // privatekey
                        ret = fileHandler.copyKeyFile(certDialog.fileName);
                    } else if (certType === 2) { // pacFile
                        ret = fileHandler.copyPacFile(certDialog.fileName);
                    }

                    // If the cert/key/pac doesn't compute, warn.
                    if (!ret) {
                        PopupUtils.open(failedToImportComponent);
                    } else {
                        certDialog.updateSignal(true);
                        certDialog.certSaved(ret);
                        PopupUtils.close(certDialog);
                    }
                }
            }
        }
    }
}
