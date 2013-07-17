import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Sound 1.0

ItemPage {
    property string title
    property variant soundsDisplay: [""]
    property variant soundsFiles: [""]

    id: soundsPage
    title: title

    flickable: scrollWidget

    UbuntuSoundPanel {
        id: backendInfo
        Component.onCompleted: buildSoundsValues(listSounds("/usr/share/sounds/ubuntu/stereo"))
    }

    function buildSoundsValues(sounds)
    {
        var tempdisplay = [];
        var tempfiles = [];
        for (var n = 0; n < sounds.length; n++) {
            tempdisplay[n]=sounds[n].split(".").slice(0,-1).join(" ");
            tempfiles[n]=sounds[n];
        }
        soundsDisplay = tempdisplay
        soundsFiles = tempfiles
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height

        ListItem.ValueSelector {
            expanded: true
            // TODO: There is no way to have a ValueSelector always expanded
            onExpandedChanged: expanded = true
            values: soundsDisplay
            onSelectedIndexChanged: print(soundsFiles[selectedIndex]) // TODO: write configuration
        }
    }
}
