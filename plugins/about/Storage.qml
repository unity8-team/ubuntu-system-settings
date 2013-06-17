import QtQuick 2.0
import Ubuntu.Components 0.1
import SystemSettings 1.0
import Ubuntu.Components.ListItems 0.1 as ListItem

ItemPage {
    id: storagePage

    title: i18n.tr("Storage")
    flickable: scrollWidget
    property var spaceColors: ["white", "red", "blue"]
    property var spaceLabels: [i18n.tr("Free space"), i18n.tr("Used by Ubuntu"),
        i18n.tr("Used by apps")]
    property var spaceValues: ["31.4 GB", "19.6 GB", "13.0 GB"]

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: columnId.height

        Column {
            id: columnId
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.SingleValue {
                id: diskItem
                text: i18n.tr("Total storage")
                value: "64.0GB"   // TODO: read value from the device
                showDivider: false
            }

            StorageBar {
                barHeight: 25
                colors: spaceColors
            }

            Repeater {
                model: spaceColors
                Item {
                    height: 19
                    width: parent.width*0.9
                    anchors.horizontalCenter: parent.horizontalCenter
                    Row {
                        spacing: units.gu(1)

                        Rectangle {
                            width: 15
                            height: 15
                            border.width: 1
                            color: modelData
                        }
                        Label { text: spaceLabels[index] }
                    }
                    Label {
                        anchors.right: parent.right
                        text: spaceValues[index]
                    }
                }
            }
        }
    }
}
