import GSettings 1.0
import QtQuick 2.4
import QtMultimedia 5.6
import SystemSettings 1.0
import Ubuntu.Content 1.3
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.SystemSettings.Sound 1.0
import QMenuModel 0.1

import "utilities.js" as Utilities

ItemPage {
    property variant soundDisplayNames:
        Utilities.buildSoundValues(soundFileNames)
    property variant soundFileNames: refreshSoundFileNames()
    property bool showStopButton: false
    property int soundType // 0: ringtone, 1: message
    property string soundsDir
    property var activeTransfer

    onSoundFileNamesChanged: {
        soundDisplayNames = Utilities.buildSoundValues(soundFileNames)
        updateSelectedIndex()
    }

    id: soundsPage
    flickable: scrollWidget

    function refreshSoundFileNames() {
        var customDir = mountPoint + "/custom/usr/share/" + soundsDir;
        if (soundType === 0)
            return backendInfo.listSounds([soundsDir, customDir, backendInfo.customRingtonePath])
        return backendInfo.listSounds([soundsDir, customDir])
    }

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

    QDBusActionGroup {
        id: soundActionGroup
        busType: DBus.SessionBus
        busName: "com.canonical.indicator.sound"
        objectPath: "/com/canonical/indicator/sound"

        Component.onCompleted: start()
    }

    Audio {
        id: soundEffect
        audioRole: MediaPlayer.alert
    }

    function setRingtone(path) {
        if (soundType == 0) {
            soundSettings.incomingCallSound = path
            backendInfo.incomingCallSound = path
        } else if (soundType == 1) {
            soundSettings.incomingMessageSound = path
            backendInfo.incomingMessageSound = path
        }
        soundFileNames = refreshSoundFileNames()
        previewTimer.start()
        soundEffect.source = path
        soundEffect.play()
    }

    function updateSelectedIndex() {
        if (soundType == 0)
            soundSelector.selectedIndex =
                    Utilities.indexSelectedFile(soundFileNames,
                        backendInfo.incomingCallSound)
        else if (soundType == 1)
            soundSelector.selectedIndex =
                    Utilities.indexSelectedFile(soundFileNames,
                        backendInfo.incomingMessageSound)
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: selectorColumn.height + stopItem.height
        boundsBehavior: (contentHeight > height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: selectorColumn
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.ItemSelector {
                id: soundSelector
                expanded: true
                model: soundDisplayNames
                selectedIndex: {
                    updateSelectedIndex()
                }
                onDelegateClicked: {
                    setRingtone(soundFileNames[index])
                }
            }

            ListItem.Standard {
                id: customRingtone
                text: i18n.tr("Custom Ringtone")
                visible: soundType === 0
                progression: true
                onClicked: pageStack.addPageToNextColumn(soundsPage, picker)
            }
        }
    }

    ListItem.SingleControl {
        id: stopItem
        anchors.bottom: parent.bottom
        control: AbstractButton {
            id: stopButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            focus: false
            width: height
            height: units.gu(4)
            enabled: soundEffect.playbackState == Audio.PlayingState
            visible: enabled

            onClicked: soundEffect.stop()

            Rectangle {
                anchors.fill: parent
                radius: width * 0.5
                border.color: UbuntuColors.warmGrey
                border.width: 1
            }

            Rectangle {
                width: parent.height * 0.4
                height: width
                smooth: true
                anchors {
                    verticalCenter: parent.verticalCenter
                    horizontalCenter: parent.horizontalCenter
                }
                color: UbuntuColors.warmGrey
            }
        }
        Rectangle {
            anchors.fill: parent
            z: parent.z - 1
            visible: stopButton.visible
            color: Theme.palette.normal.background
        }
    }

    Timer {
        id: previewTimer
        onTriggered: soundEffect.stop()
        interval: 30000
    }

    Connections {
        id: contentHubConnection
        property var ringtoneCallback
        target: activeTransfer ? activeTransfer : null
        onStateChanged: {
            if (activeTransfer.state === ContentTransfer.Charged) {
                if (activeTransfer.items.length > 0) {
                    var item = activeTransfer.items[0];
                    var toneUri;
                    if (item.move(backendInfo.customRingtonePath)) {
                        toneUri = item.url;
                    } else {
                        toneUri = backendInfo.customRingtonePath + "/" + item.url.toString().split("/").splice(-1,1);
                    }
                    ringtoneCallback(toneUri);
                }
            }
        }
    }

    Page {
        id: picker
        visible: false
        title: i18n.tr("Choose from")

        ContentPeerPicker {
            id: peerPicker
            visible: parent.visible
            handler: ContentHandler.Source
            contentType: ContentType.Music
            showTitle: false

            onPeerSelected: {
                pageStack.removePages(soundsPage);
                // requests an active transfer from peer
                function startContentTransfer(callback) {
                    if (callback)
                        contentHubConnection.ringtoneCallback = callback
                    var transfer = peer.request();
                    if (transfer !== null) {
                        soundsPage.activeTransfer = transfer;
                    }
                }
                peer.selectionType = ContentTransfer.Single;
                startContentTransfer(function(uri) {
                    setRingtone(uri.toString().replace("file:///", "/"));
                });
            }

            onCancelPressed: pageStack.removePages(soundsPage);
        }
    }

    ContentTransferHint {
        anchors.fill: parent
        activeTransfer: soundsPage.activeTransfer
    }

}
