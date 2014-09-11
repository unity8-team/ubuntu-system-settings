import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.SystemSettings.StorageAbout 1.0

ItemPage {
    property string binary;
    property string license: backendInfo.licenseInfo(binary);

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
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Label {
            id: textId
            text: license ? license : i18n.tr("Sorry, this license could not be displayed.")
            width: scrollWidget.width
            wrapMode: Text.WordWrap
        }
    }
}

