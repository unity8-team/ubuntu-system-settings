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

Column {
    id: selectSourcePage
    anchors.fill: parent

    property string defaultBackground
    property string current

    signal save (string uri)

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
                    mainPage.pageStack.push(Qt.resolvedUrl("Preview.qml"), {uri: uri});
                    console.warn('WallpaperGrid: pushed Preview.qml with uri:', uri);
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
                    mainPage.pageStack.push(Qt.resolvedUrl("Preview.qml"), {uri: uri});
                    mainPage.selectedItemConnection.target = mainPage.pageStack.currentPage;
                }
            }
            ListItem.Empty {}
        }
    }

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
