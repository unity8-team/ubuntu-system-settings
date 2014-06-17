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
import Ubuntu.SystemSettings.Background 1.0
import "utilities.js" as Utilities

ItemPage {
    id: mainPage

    objectName: "backgroundPage"

    flickable: sources

    title: i18n.tr("Background")

    signal save (string uri)

    /* TODO: For now hardcoded paths, later we'll use GSettings */
    property string defaultBackground:
        mainPage.width >= units.gu(60) ?
            "/usr/share/unity8/graphics/tablet_background.jpg" :
            "/usr/share/unity8/graphics/phone_background.jpg"

    property string welcomeBackground: backgroundPanel.backgroundFile

    property var activeTransfer

    // Action to import image
    Action {
        id: selectDefaultPeer
        text: i18n.tr("Photo/Image")
        iconName: "import-image"

        // when action has been activated, request a transfer, providing
        // a callback that pushes the preview stack
        onTriggered: {
            startContentTransfer(function(uri) {
                pageStack.push(Qt.resolvedUrl("Preview.qml"), {uri: uri});
                // set Connection target
                selectedItemConnection.target = pageStack.currentPage;
            });
        }
    }

    tools: ToolbarItems {
        ToolbarButton {
            action: selectDefaultPeer
        }
        opened: true
        locked: true
    }

    // qml bindings for background stuff
    UbuntuBackgroundPanel {
        id: backgroundPanel
    }

    Flickable {
        id: sources
        anchors.fill: parent
        visible: true
        contentHeight: sourceColumn.height + sourceColumn.anchors.bottomMargin

        Column {
            id: sourceColumn
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(1)

            WallpaperGrid {
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

            // WallpaperGrid {
            //     id: customGrid
            //     anchors.left: parent.left
            //     anchors.right: parent.right
            //     columns: 3
            //     bgmodel: backgroundPanel.customBackgrounds
            //     backgroundPanel: backgroundPanel
            //     title: i18n.tr("Custom")
            //     current: welcomeBackground
            //     editable: true
            //     onSelected: {
            //         pageStack.push(Qt.resolvedUrl("Preview.qml"), {uri: uri});
            //         selectedItemConnection.target = pageStack.currentPage
            //     }
            // }

            ListItem.Empty {}

            Connections {
                id: contentHubConnection
                property var imageCallback
                target: mainPage.activeTransfer ? mainPage.activeTransfer : null
                onStateChanged: {
                    if (mainPage.activeTransfer.state === ContentTransfer.Charged) {
                        if (mainPage.activeTransfer.items.length > 0) {
                            var imageUrl = mainPage.activeTransfer.items[0].url;
                            imageCallback(imageUrl);
                        }
                    }
                }
            }

            ContentPeer {
                id: peer
                contentType: ContentType.Pictures
                handler: ContentHandler.Source
                selectionType: ContentTransfer.Single
            }

            ContentStore {
                id: appStore
                scope: ContentScope.App
            }

            // requests an active transfer from peer
            function startContentTransfer(callback) {
                if (callback)
                    contentHubConnection.imageCallback = callback
                var transfer = peer.request(appStore);
                if (transfer !== null) {
                    mainPage.activeTransfer = transfer;
                }
            }


        }
    }

    // set up connections
    Connections {
        id: selectedItemConnection
        onSave: {
            console.warn('selectedItemConnection.onSave', target.uri);
            Utilities.setBackground(target.uri)
        }
        onStateChanged: {
            console.warn('selectedItemConnection.onStateChanged: new state: ', target.state);
            var trans = mainPage.activeTransfer;
            if (target.state === "saved") {
                save(target.uri);
                console.warn('selectedItemConnection.onStateChanged: save', target.uri);
                console.warn('selectedItemConnection.onStateChanged: activeTransfer', trans);

                // if a transfer is done, clean up
                if (trans && trans.state === ContentTransfer.Collected) {
                    trans.state = ContentTransfer.Finalized;
                }
            }
            // if we did an import, clean up
            if ((target.state === "cancelled") &&
                (trans && trans.state === ContentTransfer.Collected)) {
                backgroundPanel.rmFile(target.uri);
                trans.state = ContentTransfer.Finalized;
            }
        }
    }

}
