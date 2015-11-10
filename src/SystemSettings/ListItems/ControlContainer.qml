import QtQuick 2.4

Item {
    id: container
    property Item control
    width: control ? control.width : undefined
    height: control ? control.height : undefined
    onControlChanged: if (control) control.parent = container
}
