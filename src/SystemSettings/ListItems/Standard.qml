import Ubuntu.Components 1.3

ListItem {
    id: root
    default property alias slots: layoutItem.children
    property alias         layout: layoutItem
    property string        text
    property bool          showDivider: true

    height: layoutItem.height + (divider.visible ? divider.height : 0)
    divider.visible: showDivider

    ListItemLayout {
        id: layoutItem
        title.text: root.text
        title.opacity: enabled ? 1 : 0.5
    }
}
