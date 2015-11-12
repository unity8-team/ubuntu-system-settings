import QtQuick 2.4
import Ubuntu.Components 1.3

ListItem {
    id: root
    default property alias slots: layoutItem.children
    property alias         layout: layoutItem
    property bool          showDivider: true

    height: layoutItem.height + (divider.visible ? divider.height : 0)
    divider.visible: showDivider

    SlotsLayout {
        id: layoutItem
    }
}
