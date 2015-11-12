import QtQuick 2.4
import "." as SettingsListItems
import Ubuntu.Components 1.3

SettingsListItems.Standard {
    id: base
    property alias value: value.text
    property alias progressionVisible: progression.visible

    Label {
        id: value
        horizontalAlignment: Text.AlignRight
    }

    ProgressionSlot {
        id: progression
    }
}
