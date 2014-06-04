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
 * Ken VanDine <ken.vandine@canonical.com>
 *
*/

import QtQuick 2.0
import GSettings 1.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Content 0.1
import Ubuntu.SystemSettings.Background 1.0

ItemPage {
    id: selectSourcePage
    flickable: sourceSelector
    anchors.fill: parent

    property bool homeScreen
    property bool useSame
    property var activeTransfer
    property var store
    property string defaultBackground
    property string current
    signal save (bool homeScreen, string uri)

    title: useSame ? i18n.tr("Choose background") : homeScreen ?
                         i18n.tr("Home screen") : i18n.tr("Welcome screen")

    Action {
        id: selectDefaultPeer
        text: i18n.tr("Photo/Image")
        iconName: "import-image"
        onTriggered: {
            startContentTransfer(function(uri) {
                pageStack.push(Qt.resolvedUrl("Preview.qml"), {uri: uri});
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

    UbuntuBackgroundPanel {
        id: backgroundPanel
    }

    Flickable {
        id: sourceSelector
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
                current: selectSourcePage.current
                onSelected: {
                    pageStack.push(Qt.resolvedUrl("Preview.qml"), {uri: uri});
                    selectedItemConnection.target = pageStack.currentPage;
                }
            }

            WallpaperGrid {
                id: customGrid
                anchors.left: parent.left
                anchors.right: parent.right
                columns: 3
                bgmodel: backgroundPanel.customBackgrounds
                backgroundPanel: backgroundPanel
                title: i18n.tr("Custom")
                current: selectSourcePage.current
                editable: true
                onSelected: {
                    pageStack.push(Qt.resolvedUrl("Preview.qml"), {uri: uri});
                    selectedItemConnection.target = pageStack.currentPage;
                }
            }

            Connections {
                id: selectedItemConnection
                onStateChanged: {
                    if (target.state === "saved") {
                        save(homeScreen, target.uri);
                        if (activeTransfer.state === ContentTransfer.Collected)
                            activeTransfer.state = ContentTransfer.Finalized;
                    }
                    if ((target.state === "cancelled") &&
                        (activeTransfer.state === ContentTransfer.Collected)) {
                        backgroundPanel.rmFile(target.uri);
                        activeTransfer.state = ContentTransfer.Finalized;
                    }
                }
            }
            ListItem.Empty {}
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

    ContentPeer {
        id: peer
        contentType: ContentType.Pictures
        handler: ContentHandler.Source
        selectionType: ContentTransfer.Single
    }

    ContentStore {
        id: appStore
        scope: ContentScope.App
        uri: "file://" + backgroundPanel.customBackgroundFolder
    }

    function startContentTransfer(callback) {
        if (callback)
            contentHubConnection.imageCallback = callback
        var transfer = peer.request(appStore);
        if (transfer !== null) {
            activeTransfer = transfer;
        }
    }
}
