import QtQuick 2.4
import Ubuntu.Components 1.2
import Ubuntu.Layouts 1.0

ListItem {
    id: listItem
    height: layout.height + divider.height

    ListItemLayout {
        id: layout
        title.text: "Hello..."
        title.color: UbuntuColors.Orange
        subtitle.text: "...world!"

        Rectangle {
            SlotsLayout.position: SlotsLayout.Leading
            color: "pink"
            height: units.gu(6)
            width: height
        }

        Item {
            id: slot
            width: secondLabel.width
            height: parent.height

            //as we want to position labels to align with title and subtitle
            SlotsLayout.overrideVerticalPositioning: true

            Label {
                id: firstLabel
                anchors.right: secondLabel.right
                text: "19:17"
                fontSize: "small"
                y: layout.mainSlot.y + layout.title.y
                   + layout.title.baselineOffset - baselineOffset
            }

            Label {
                id: secondLabel
                text: "Outgoing"
                fontSize: "small"
                y: layout.mainSlot.y + layout.subtitle.y
                   + layout.subtitle.baselineOffset - baselineOffset
            }
        }
    }
}
