import QtQuick 2.0
import Ubuntu.Components 1.1

Item {
    property bool ready: false
    anchors.horizontalCenter: parent.horizontalCenter
    height: units.gu(5)
    width: parent.width - units.gu(4)

    UbuntuShape {
        //border.width: 1
        color: "white"
        clip: true
        height: units.gu(3)
        width: parent.width
        image: ses
    }

    ShaderEffectSource {
        id: ses
        sourceItem: row
        width: 1
        height: 1
        hideSource: true
    }

    Row {
        id: row
        visible: false

        anchors.fill: parent

        Repeater {
            model: spaceColors

            Rectangle {
                color: ready ? modelData : UbuntuColors.warmGrey
                height: parent.height
                width: spaceValues[index] / diskSpace * parent.width
                Behavior on color {
                    ColorAnimation {
                        duration: UbuntuAnimation.SlowDuration
                        easing: UbuntuAnimation.StandardEasing
                    }
                }
            }
        }

        Rectangle {
            color: "white"
            height: parent.height
            width: parent.width
        }
    }
}
