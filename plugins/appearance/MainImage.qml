import QtQuick 2.0

import Ubuntu.Components 0.1

UbuntuShape {
    id: root

    property alias source: mainImage.source
    property string altSource
    property bool enabled
    signal clicked

    anchors {
        top: welcomeLabel.bottom
        left: parent.left
        margins: units.gu(2)
    }

    height: parent.height * 0.4
    width: parent.width * 0.45

    enabled: true

    image: Image {
        id: mainImage
        height: parent.height
        width: parent.width
        fillMode: Image.PreserveAspectCrop
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }

    states: State {
        name: "disabled"
        when: !root.enabled
        PropertyChanges {
            target : root
            image : altImage
        }
        PropertyChanges {
            target: mouseArea
            enabled: false
        }
        PropertyChanges {
            target : mainImage
            source : altSource
        }
    }

    transitions : Transition {
        NumberAnimation {
            properties : "source"
            easing.type : Easing.InOutQuad
            duration : 1000
        }
    }
}
