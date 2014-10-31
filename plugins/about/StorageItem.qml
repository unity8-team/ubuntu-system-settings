import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    property string label
    property color colorName
    property string value
    property bool ready: false

    height: units.gu(3)
    width: parent.width-units.gu(4)
    anchors.horizontalCenter: parent.horizontalCenter
    Row {
        spacing: units.gu(1)

        UbuntuShape {
            anchors.verticalCenter: parent.verticalCenter
            width: units.gu(3)
            height: units.gu(2)
            color: colorName
        }
        Label { text: label }
    }
    Label {
        id: sizelabel
        objectName: "sizeLabel"
        anchors.right: parent.right
        text: Utilities.formatSize(value)
        visible: ready
    }
    ActivityIndicator {
        anchors.right: parent.right
        visible: !ready
        running: true
    }
}
