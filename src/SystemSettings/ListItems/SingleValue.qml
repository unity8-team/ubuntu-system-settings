import QtQuick 2.4
import "." as SettingsListItems
import Ubuntu.Components 1.3

Standard {
    id: base
    property alias value: value.text

    Label {
        id: value
        horizontalAlignment: Text.AlignRight
    }
}
