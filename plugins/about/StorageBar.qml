import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    property var colors

    anchors.horizontalCenter: parent.horizontalCenter
    height: units.gu(5)
    width: parent.width - units.gu(4)

    Rectangle {
        border.width: 1
        color: colors[0]
        height: units.gu(3)
        width: parent.width

        Row {
            anchors.centerIn: parent
            height: parent.height-2
            width: parent.width-2
            Rectangle {
                color: colors[1]
                height: parent.height
                width: 50
            }
            Rectangle {
                color: colors[2]
                height: parent.height
                width: 30
            }
        }

    }
}
