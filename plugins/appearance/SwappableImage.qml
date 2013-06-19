import QtQuick 2.0

UbuntuSwappableImage {
    height: parent.height * 0.4
    width: parent.width * 0.43

    anchors {
        margins: units.gu(2)
    }

    onEnabledChanged: swapImage()
}
