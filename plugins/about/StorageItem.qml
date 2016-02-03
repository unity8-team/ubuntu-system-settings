import QtQuick 2.4
import Ubuntu.Components 1.3

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
            backgroundColor: colorName
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
        running: visible
    }
}
