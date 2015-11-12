import QtQuick 2.4
import "." as SettingsListItems

SettingsListItems.Standard {
    id: base
    property alias value: value.text

    Label {
        id: value
        horizontalAlignment: Text.AlignRight
    }
}
