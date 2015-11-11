import Ubuntu.Components 1.3

BaseListItem {
    id: base
    height: layout.height + divider.height
    property alias progressionVisible: progression.visible

    BaseLayout {
        id: layout
        title.text: base.text

        ProgressionSlot {
            id: progression
        }
    }
}
