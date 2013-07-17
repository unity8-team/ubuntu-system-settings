import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Sound 1.0

ItemPage {
    property string title;

    title: title

    UbuntuSoundPanel {
        id: backendInfo
    }
    ListItem.ValueSelector {
        expanded: true
        // TODO: There is no way to have a ValueSelector always expanded
        onExpandedChanged: expanded = true
        values: backendInfo.listSounds("/usr/share/sounds/ubuntu/stereo")
    }
}
