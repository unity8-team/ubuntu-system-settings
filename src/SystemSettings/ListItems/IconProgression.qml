import QtQuick 2.4
import "." as SettingsListItems
import Ubuntu.Components 1.3

SettingsListItems.Icon {
    id: base
    property alias progressionVisible: progression.visible

    ProgressionSlot {
        id: progression
    }
}
