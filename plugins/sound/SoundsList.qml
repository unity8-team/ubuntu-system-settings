import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Sound 1.0

ItemPage {
    property string title
    property variant soundDisplayNames
    property variant soundFileNames
    property bool silentModeOn: false

    id: soundsPage
    title: title

    UbuntuSoundPanel {
        id: backendInfo
        Component.onCompleted:
            buildSoundValues(listSounds("/usr/share/sounds/ubuntu/stereo"))
    }

    function buildSoundValues(sounds)
    {
        soundDisplayNames = sounds.map(function (sound) {
            return sound.split('/').pop().split('.').slice(0,-1).join(" ");
        })
        soundFileNames = sounds
    }

    Column {
        id: columnId
        anchors.left: parent.left
        anchors.right: parent.right

        SilentModeWarning {
            id: silentId
            silentMode: silentModeOn
        }

        ListItem.SingleControl {
            id: listId
            control: Button {
                text: i18n.tr("Stop playing")
                width: parent.width - units.gu(4)
            }
            visible: !silentModeOn
        }
    }

    Flickable {
        width: parent.width
        contentHeight: contentItem.childrenRect.height
        anchors.top: columnId.bottom
        anchors.bottom: soundsPage.bottom
        clip: true

        ListItem.ValueSelector {
            id: soundSelector
            expanded: true
            // TODO: There is no way to have a ValueSelector always expanded
            onExpandedChanged: expanded = true
            values: soundDisplayNames
            onSelectedIndexChanged:
                print(soundFileNames[selectedIndex]) // TODO: write configuration
        }
    }
}
