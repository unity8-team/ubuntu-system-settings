import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Layouts 1.0

Standard {
    id: root

    property bool   progression: false
    property bool   showDivider: false
    property alias  control: controlContainer.control
    property string iconName
    property url    iconSource
    property bool   iconFrame: false
    property string text
    property bool   removeable: false
    property bool   confirmRemoval: false
    property bool   leadWithControl: false

    signal itemRemoved
    divider.visible: showDivider

    ListItemLayout {
        id: layout

        title.text: root.text
        title.opacity: enabled ? 1 : 0.5

        Icon {
            id: icon
            SlotsLayout.position: leadWithControl ? SlotsLayout.Trailing : SlotsLayout.Leading
            visible: (name !== "" || source.toString() !== "")
            name: root.iconName ? root.iconName : ""
            height: Math.min(units.gu(5), layout.height - units.gu(1))
            width: height
        }

        Item {
            id: controlContainer
            property Item control
            SlotsLayout.position: leadWithControl ? SlotsLayout.Leading : SlotsLayout.Trailing
            width: control ? control.width : undefined
            height: control ? control.height : undefined
            visible: control
            onControlChanged: if (control) control.parent = controlContainer
        }

        ProgressionSlot {
            visible: root.progression
        }
    }

    onIconSourceChanged: icon.source = iconSource
}
