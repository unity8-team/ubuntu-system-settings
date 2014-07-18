import GSettings 1.0
import QtQuick 2.0
import QtMultimedia 5.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Sound 1.0

import "utilities.js" as Utilities

ItemPage {
    property variant soundDisplayNames:
        Utilities.buildSoundValues(soundFileNames)
    property variant soundFileNames:
        backendInfo.listSounds(soundsDir).map(function (sound) {
            return soundsDir+sound
        })
    property bool showStopButton: false
    property int soundType // 0: ringtone, 1: message
    property string soundsDir

    id: soundsPage

    UbuntuSoundPanel {
        id: backendInfo
        onIncomingCallSoundChanged: {
            if (soundType == 0)
                soundSelector.selectedIndex =
                        Utilities.indexSelectedFile(soundFileNames,
                                                    incomingCallSound)
        }
        onIncomingMessageSoundChanged: {
            if (soundType == 1)
                soundSelector.selectedIndex =
                        Utilities.indexSelectedFile(soundFileNames,
                                                    incomingMessageSound)
        }
    }

    GSettings {
        id: soundSettings
        schema.id: "com.ubuntu.touch.sound"
    }

    Audio {
        id: soundEffect
    }

    Column {
        id: columnId
        anchors.left: parent.left
        anchors.right: parent.right

        SilentModeWarning { visible: backendInfo.silentMode }

        ListItem.SingleControl {
            id: listId
            control: Button {
                text: i18n.tr("Stop playing")
                width: parent.width - units.gu(4)
                onClicked:
                    soundEffect.stop()
            }
            enabled: soundEffect.playbackState == Audio.PlayingState
            visible: showStopButton && !backendInfo.silentMode
        }
    }

    ListItem.ItemSelector {
        id: soundSelector
        anchors.top: columnId.bottom
        anchors.bottom: soundsPage.bottom
        containerHeight: height

        expanded: true
        model: soundDisplayNames
        selectedIndex: {
            if (soundType == 0)
                soundSelector.selectedIndex =
                        Utilities.indexSelectedFile(soundFileNames,
                            backendInfo.incomingCallSound)
            else if (soundType == 1)
                soundSelector.selectedIndex =
                        Utilities.indexSelectedFile(soundFileNames,
                            backendInfo.incomingMessageSound)
        }
        onDelegateClicked: {
            if (soundType == 0) {
                soundSettings.incomingCallSound = soundFileNames[index]
                backendInfo.incomingCallSound = soundFileNames[index]
            } else if (soundType == 1) {
                soundSettings.incomingMessageSound = soundFileNames[index]
                backendInfo.incomingMessageSound = soundFileNames[index]
            }
            /* Only preview the file if not in silent mode */
            if (!backendInfo.silentMode) {
                soundEffect.source = soundFileNames[index]
                soundEffect.play()
            }
        }
    }
}
