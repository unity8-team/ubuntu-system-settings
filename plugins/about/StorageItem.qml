import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    property string label
    property color colorName
    property string value

    height: units.gu(3)
    width: parent.width-units.gu(4)
    anchors.horizontalCenter: parent.horizontalCenter
    Row {
        spacing: units.gu(1)

        Rectangle {
            width: units.gu(2)
            height: units.gu(2)
            border.width: units.dp(1)
            color: colorName
        }
        Label { text: label }
    }
    Label {
        anchors.right: parent.right
        text: value
    }
}
