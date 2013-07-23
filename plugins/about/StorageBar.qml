import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    anchors.horizontalCenter: parent.horizontalCenter
    height: units.gu(5)
    width: parent.width - units.gu(4)

    Rectangle {
        border.width: 1
        color: "white"
        height: units.gu(3)
        width: parent.width
        Row {
            anchors.centerIn: parent
            height: parent.height-2
            width: parent.width-2
            Repeater {
                model: spaceColors

                Rectangle {
                    color: modelData
                    height: parent.height
                    width: spaceValues[index]*1000000000 / diskSpace * parent.width // TODO: drop value hack when we get the real space
                }
            }
        }
    }
}
