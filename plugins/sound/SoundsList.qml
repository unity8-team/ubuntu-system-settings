import GSettings 1.0
import QtQuick 2.0
import QtMultimedia 5.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Sound 1.0

import "utilities.js" as Utilities

ItemPage {
    property string title
    property variant soundDisplayNames
    property variant soundFileNames
    property bool showStopButton: false
    property int soundType // 0: ringtone, 1: message

    id: soundsPage
    title: title

    UbuntuSoundPanel {
        id: backendInfo
        Component.onCompleted: {
            soundFileNames = listSounds("/usr/share/sounds/ubuntu/stereo").map(function (sound)
                             {return '/usr/share/sounds/ubuntu/stereo/'+sound})
            soundDisplayNames = Utilities.buildSoundValues(soundFileNames)
            if (soundType == 0)
                soundSelector.selectedIndex = Utilities.indexSelectedFile(soundFileNames, soundSettings.incomingCallSound)
            else if (soundType == 1)
                soundSelector.selectedIndex = Utilities.indexSelectedFile(soundFileNames, soundSettings.incomingMessageSound)
        }
    }

    GSettings {
        id: soundSettings
        schema.id: "com.ubuntu.touch.sound"
        onChanged: {
            if (soundType == 0 && key == "incomingCallSound")
                soundSelector.selectedIndex = Utilities.indexSelectedFile(soundFileNames, value)
            if (soundType == 1 && key == "incomingMessageSound")
                soundSelector.selectedIndex = Utilities.indexSelectedFile(soundFileNames, value)
        }
    }

    Audio {
        id: soundEffect
    }

    Column {
        id: columnId
        anchors.left: parent.left
        anchors.right: parent.right

        SilentModeWarning { visible: soundSettings.silentMode }

        ListItem.SingleControl {
            id: listId
            control: Button {
                text: i18n.tr("Stop playing")
                width: parent.width - units.gu(4)
                onClicked:
                    soundEffect.stop()
            }
            enabled: soundEffect.playbackState == Audio.PlayingState
            visible: showStopButton && !soundSettings.silentMode
        }
    }

    Flickable {
        width: parent.width
        contentHeight: contentItem.childrenRect.height
        anchors.top: columnId.bottom
        anchors.bottom: soundsPage.bottom
        clip: true

        ListItem.ItemSelector {
            id: soundSelector
            expanded: true
            model: soundDisplayNames
            onDelegateClicked: {
                if (soundType == 0)
                    soundSettings.incomingCallSound = soundFileNames[index]
                else if (soundType == 1)
                    soundSettings.incomingMessageSound = soundFileNames[index]
                /* Only preview the file if not in silent mode */
                if (!soundSettings.silentMode) {
                    soundEffect.source = soundFileNames[index]
                    soundEffect.play()
                }
            }
        }
    }
}
