import QtQuick 2.0
import QtQuick.Layouts 1.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Wifi 1.0

Component  {

    Dialog {
        id: certDialog

        property var certFileName;

        anchors.fill: parent

        title: i18n.tr("Add Certificate?")

        CertificateHandler {
            id: certificateHandler
        }

        TextArea {
            id : certContent
            objectName: "certContent"
            readOnly: true
            width: parent.width
            autoSize: true
            maximumLineCount: 7
            placeholderText: i18n.tr("No data available.")
            text: {certificateHandler.getCertContent(certDialog.certFileName).toString()}
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
                text: i18n.tr("Cancel")
                onClicked: { certificateHandler.removeFile(certDialog.certFileName);
                             PopupUtils.close(certDialog);
                }
            }

            Button {
                id: saveButton
                text: i18n.tr("Add Certificate")
                enabled: (certDialog.certContent.text !== [])
                onClicked: { certificateHandler.moveCertFile(certDialog.certFileName);
                             // just to be sure source file will be deleted if move was not successfull:
                             certificateHandler.removeFile(certDialog.certFileName);
                             PopupUtils.close(certDialog);
                }
            }
        }
    }
}
