import QtQuick 2.4
import Ubuntu.Components 1.3

Standard {
    id: base
    property alias iconName: icon.name
    property alias iconSource: icon.source

    Icon {
        id: icon
        width: height
        height: units.gu(2.5)
        SlotsLayout.position: SlotsLayout.First
    }
}
