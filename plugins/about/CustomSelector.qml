import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

ListItem.Base {
    property bool sortByName: true

    Item {
        id: buttonsBox
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

        Button {
            id: buttonByName
            anchors {
                rightMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
            }
            gradient: sortByName ? UbuntuColors.orangeGradient : UbuntuColors.greyGradient
            text: i18n.tr("By Name")
            width: (buttonsBox.width - units.gu(1))/2
            onClicked: sortByName = true
        }
        Button {
            id: buttonBySize
            anchors {
                verticalCenter: parent.verticalCenter
                leftMargin: units.gu(1)
                right: parent.right
            }
            gradient: !sortByName ? UbuntuColors.orangeGradient : UbuntuColors.greyGradient
            text: i18n.tr("By Size")
            width: buttonByName.width
            onClicked: sortByName = false
        }
    }
}
