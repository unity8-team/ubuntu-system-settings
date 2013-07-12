import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.StorageAbout 1.0

ItemPage {
    property string binary;

    id: licensesPage
    title: binary
    flickable: scrollWidget

    UbuntuStorageAboutPanel {
        id: backendInfo
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        anchors.margins: units.gu(2)
        contentHeight: textId.height

        Text {
            id: textId
            text: backendInfo.licenseInfo(binary)
            width: scrollWidget.width
            wrapMode: Text.WordWrap
        }
    }
}

