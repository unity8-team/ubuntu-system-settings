import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.StorageAbout 1.0

ItemPage {

    id: licensesPage
    title: i18n.tr("Software licenses")

    UbuntuStorageAboutPanel {
        id: backendInfo
    }

    ListView {
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height

        model: backendInfo.licensesList
        delegate: ListItem.Standard {
            text: modelData
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("License.qml"), {binary: modelData})
        }

    }
}
