import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Sound 1.0

ItemPage {
    property string title
    property variant soundDisplayNames
    property variant soundFileNames

    id: soundsPage
    title: title

    flickable: scrollWidget

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

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height

        ListItem.ValueSelector {
            expanded: true
            // TODO: There is no way to have a ValueSelector always expanded
            onExpandedChanged: expanded = true
            values: soundDisplayNames
            onSelectedIndexChanged:
                print(soundFileNames[selectedIndex]) // TODO: write configuration
        }
    }
}
