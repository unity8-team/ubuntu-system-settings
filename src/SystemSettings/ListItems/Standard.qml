import QtQuick 2.4
import Ubuntu.Components 1.2
import Ubuntu.Layouts 1.0

Layouts {
    id: root

    property bool   progression: false
    property alias  showDivider: item.divider
    property Item   control
    property string iconName
    property url    iconSource
    property url    iconFrame
    property string text
    property bool   visible: true
    property bool   enabled: true
    property bool   removeable: false
    property bool   confirmRemoval: false

    signal onItemRemoved()

    // layouts: [
    //     ConditionalLayout {
    //         name: "control"
    //         when: control
    //         Flow {
    //             anchors.fill: parent
    //             //[...]
    //         }
    //     },
    //     ConditionalLayout {
    //         name: "column"
    //         when: root.width > units.gu(100)
    //         Flickable {
    //             anchors.fill: parent
    //             contentHeight: column.childrenRect.height
    //             Column {
    //                 id: column
    //                 //[...]
    //             }
    //         }
    //     }
    // ]

    SlotsLayout {
        id: item

        // NOTE: mainSlot's attached properties set via JS
        mainSlot: Label {}

        Icon {
            Layouts.item: "icon"
            SlotsLayout.position: SlotsLayout.First
            visible: root.iconName || root.iconSource
        }
     
        Item {
            Layouts.item: "control"
            SlotsLayout.position: SlotsLayout.Trailing
            visible: root.control
            children: [root.control]
        }

        ProgressionSlot {
            Layouts.item: "progression"
            SlotsLayout.position: SlotsLayout.End
            visible: root.progression
        }

        Component.onCompleted: {
            mainSlot.SlotsLayout.position =
                Qt.binding(function() { return SlotsLayout.First })
            mainSlot.Layouts.item =
                Qt.binding(function() { return "label" })
        }
    }
}

// ListItem {
//     id: item

//     property bool   progression: false
//     property alias  showDivider: item.divider
//     property Item   control
//     property string iconName
//     property url    iconSource
//     property url    iconFrame
//     property string text
//     property bool   visible: true
//     property bool   enabled: true
//     property bool   removeable: false
//     property bool   confirmRemoval: false

//     signal onItemRemoved()

//     height: layout.height + divider.heightl

//     // ListItemLayout {

//     // }
// }

// ListItem {
//     id: listItem
//     height: layout.height + divider.height

//     ListItemLayout {
//         id: layout
//         title.text: "Hello..."
//         title.color: UbuntuColors.Orange
//         subtitle.text: "...world!"

//         Rectangle {
//             SlotsLayout.position: SlotsLayout.Leading
//             color: "pink"
//             height: units.gu(6)
//             width: height
//         }

//         Item {
//             id: slot
//             width: secondLabel.width
//             height: parent.height

//             //as we want to position labels to align with title and subtitle
//             SlotsLayout.overrideVerticalPositioning: true

//             Label {
//                 id: firstLabel
//                 anchors.right: secondLabel.right
//                 text: "19:17"
//                 fontSize: "small"
//                 y: layout.mainSlot.y + layout.title.y
//                    + layout.title.baselineOffset - baselineOffset
//             }

//             Label {
//                 id: secondLabel
//                 text: "Outgoing"
//                 fontSize: "small"
//                 y: layout.mainSlot.y + layout.subtitle.y
//                    + layout.subtitle.baselineOffset - baselineOffset
//             }
//         }
//     }
// }
