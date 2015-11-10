import Ubuntu.Components 1.3

BaseListItem {
    id: base
    property alias  control: controlContainer.control
    property bool   leadWithControl: false

    BaseLayout {
        title.text: base.text

        ControlContainer {
            id: controlContainer
            SlotsLayout.position: leadWithControl ? SlotsLayout.Leading : SlotsLayout.Trailing
        }
    }
}
