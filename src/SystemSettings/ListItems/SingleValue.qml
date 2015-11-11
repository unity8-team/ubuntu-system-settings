import Ubuntu.Components 1.3

BaseListItem {
    id: base
    property alias value: value.text
    height: layout.height + divider.height

    BaseLayout {
        id: layout
        title.text: base.text

        Label {
            id: value
            horizontalAlignment: Text.AlignRight
        }
    }
}
