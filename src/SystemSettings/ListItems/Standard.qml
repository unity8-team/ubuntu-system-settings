import Ubuntu.Components 1.3

BaseListItem {
    id: base
    height: layout.height + divider.height

    BaseLayout {
        id: layout
        title.text: base.text
    }
}
