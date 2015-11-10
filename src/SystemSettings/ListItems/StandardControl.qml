import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Layouts 1.0
import SystemSettings.ListItems 1.0 as ListItems

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
