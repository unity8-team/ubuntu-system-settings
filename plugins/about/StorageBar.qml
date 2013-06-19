import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    property int barHeight
    property var colors

    anchors.horizontalCenter: parent.horizontalCenter
    width: parent.width*0.9
    height: barHeight + 2*units.gu(1)

    Row {
        id: spaceRow
        Rectangle {
            color: colors[1]
            height: barHeight
            width: 50
        }
        Rectangle {
            color: colors[2]
            height: barHeight
            width: 30
        }
    }
    Rectangle {
        color: colors[0]
        anchors.left: spaceRow.right
        anchors.right: parent.right
        height: barHeight
    }
}
