import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Phone 1.0

ItemPage {
    title: i18n.tr("Wi-Fi")

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.Standard {
            text: i18n.tr("Auto-join previous networks")
            control: Switch {
                checked: true
                enabled: false
            }
        }

        ListItem.Standard {
            text: i18n.tr("Prompt when not connected")
            control: Switch {
                checked: true
                enabled: false
            }
        }

        ListItem.Caption {
            text: i18n.tr("List available wi-fi networks, if any, when you're using cellular data.")
        }

    }
}
