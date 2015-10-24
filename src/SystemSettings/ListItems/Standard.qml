import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Layouts 1.0

ListItem {
    id: root

    property bool   progression: false
    property bool   showDivider: false
    property alias  control: controlContainer.control
    property string iconName
    property url    iconSource
    property url    iconFrame
    property alias  text: label.text
    property bool   enabled: true
    property bool   removeable: false
    property bool   confirmRemoval: false

    // signal itemRemoved()

    SlotsLayout {
        id: layout

        // NOTE: mainSlot's attached properties set via JS
        mainSlot: Label {
            id: label
            height: units.gu(6)
        }

        Icon {
            // Layouts.item: "icon"
            SlotsLayout.position: SlotsLayout.First
            visible: (typeof root.iconName !== "undefined" ||
                      typeof root.iconSource === "undefined")
            height: label.height
        }

        Item {
            id: controlContainer
            property Item control
            // Layouts.item: "control"
            SlotsLayout.position: SlotsLayout.Trailing
            width: control ? control.width : undefined
            height: control ? control.height : undefined
            visible: control
            onControlChanged: {
                if (control) control.parent = controlContainer;
            }
        }

        ProgressionSlot {
            // Layouts.item: "progression"
            visible: root.progression
        }

        Component.onCompleted: {
            mainSlot.SlotsLayout.position =
                Qt.binding(function() { return SlotsLayout.First })
            // mainSlot.Layouts.item =
            //     Qt.binding(function() { return "label" })
        }
    }
}
