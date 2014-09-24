/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Iain Lane <iain.lane@canonical.com>
 *
*/

import QtQuick 2.0
import GSettings 1.0
import SystemSettings 1.0
import Ubuntu.Content 0.1
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import Ubuntu.SystemSettings.Background 1.0
import "utilities.js" as Utilities

ItemPage {
    id: mainPage
    objectName: "backgroundPage"
    flickable: sources
    title: i18n.tr("Background")

    signal save (string uri)

    /* TODO: For now hardcoded paths, later we'll use GSettings */
    /* TODO: fix bug where rotating in uss will change default
    background to tablet_back… thus losing track of phone_back… */
    property string defaultBackground:
        mainPage.width >= units.gu(60) ?
            "/usr/share/unity8/graphics/tablet_background.jpg" :
            "/usr/share/unity8/graphics/phone_background.jpg"

    /* If there is no uri then use the default */
    property string welcomeBackground: (backgroundPanel.backgroundFile === "file:") ?
                                           "file:///usr/share/unity8/graphics/phone_background.jpg" :
                                           backgroundPanel.backgroundFile

    property var activeTransfer

    // Action to import image
    Action {
        id: selectPeer
        // when action has been activated, push the picker on the stack
        onTriggered: {                        
            pageStack.push(picker);
        }
    }

    // qml bindings for background stuff
    UbuntuBackgroundPanel {
        id: backgroundPanel
    }

    Flickable {
        id: sources

        anchors {
            fill: parent
            topMargin: units.gu(2)
        }
        visible: true
        contentHeight: sourceColumn.height + sourceColumn.anchors.bottomMargin

        Column {
            id: sourceColumn
            anchors {
                left: parent.left
                right: parent.right
            }

            WallpaperGrid {
                id: uArtGrid
                objectName: "UbuntuArtGrid"
                anchors.left: parent.left
                anchors.right: parent.right
                columns: 3
                bgmodel: {
                    // Make a shallow copy
                    var backgrounds = backgroundPanel.ubuntuArt.slice(0)
                    if (backgroundPanel.fileExists(defaultBackground))
                        backgrounds.push(Qt.resolvedUrl(defaultBackground))
                    return backgrounds
                }
                backgroundPanel: backgroundPanel
                title: i18n.tr("Ubuntu Art")
                current: welcomeBackground
                onSelected: {
                    pageStack.push(Qt.resolvedUrl("Preview.qml"), {uri: uri});
                    selectedItemConnection.target = pageStack.currentPage;
                }
            }

            WallpaperGrid {
                id: customGrid
                objectName: "customArtGrid"
                anchors.left: parent.left
                anchors.right: parent.right
                columns: 3
                bgmodel: backgroundPanel.customBackgrounds
                backgroundPanel: backgroundPanel
                title: i18n.tr("Custom")
                current: welcomeBackground
                editable: true
                isCustom: true
                onSelected: {
                    pageStack.push(Qt.resolvedUrl("Preview.qml"), {uri: uri});
                    selectedItemConnection.target = pageStack.currentPage
                }
            }

            ListItem.ThinDivider {}

        }
    }

    Connections {
        id: contentHubConnection
        property var imageCallback
        target: activeTransfer ? activeTransfer : null
        onStateChanged: {
            if (activeTransfer.state === ContentTransfer.Charged) {
                if (activeTransfer.items.length > 0) {
                    var imageUrl = activeTransfer.items[0].url;
                    imageCallback(imageUrl);
                }
            }
        }
    }    

    // set up connections
    Connections {
        id: selectedItemConnection
        onSave: {
            Utilities.setBackground(target.uri)
        }
        onStateChanged: {
            var trans = mainPage.activeTransfer;
            if (target.state === "saved") {
                save(target.uri);

                // if a transfer is done, clean up
                if (trans && trans.state === ContentTransfer.Collected) {
                    trans.state = ContentTransfer.Finalized;
                }
            }

            if ((target.state === "cancelled") &&
                (trans && trans.state === ContentTransfer.Collected)) {

                if (target.imported) {
                    // if we just did an import, remove the image if the user
                    // cancels
                    backgroundPanel.rmFile(target.uri);
                } else {
                    backgroundPanel.prepareBackgroundFile(target.uri, true);
                }
                trans.state = ContentTransfer.Finalized;
            }
        }
    }

    Page {
        id: picker
        visible: false

        ContentStore {
            id: appStore
            scope: ContentScope.App
        }

        ContentPeerPicker {
            id: peerPicker
            visible: parent.visible
            handler: ContentHandler.Source
            contentType: ContentType.Pictures

            onPeerSelected: {
                pageStack.pop();
                // requests an active transfer from peer
                function startContentTransfer(callback) {
                    if (callback)
                        contentHubConnection.imageCallback = callback
                    var transfer = peer.request(appStore);
                    if (transfer !== null) {
                        mainPage.activeTransfer = transfer;
                    }
                }
                peer.selectionType = ContentTransfer.Single;
                // when peer has been selected, request a transfer, providing
                // a callback that pushes the preview stack
                startContentTransfer(function(uri) {
                    pageStack.push(Qt.resolvedUrl("Preview.qml"), {
                        uri: uri, imported: true
                    });
                    // set Connection target
                    selectedItemConnection.target = pageStack.currentPage;
                });
            }

            onCancelPressed: pageStack.pop();
        }
    }

    ContentTransferHint {
        anchors.fill: parent
        activeTransfer: mainPage.activeTransfer
    }
}
