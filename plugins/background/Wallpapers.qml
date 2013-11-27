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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Content 0.1
import SystemSettings 1.0
import Ubuntu.SystemSettings.Background 1.0

ItemPage {
    id: selectSourcePage
    flickable: sourceSelector
    anchors.fill: parent

    property bool homeScreen: true
    property var gsettings
    property var mainPage
    property string ubuntuArtDir: "/usr/share/backgrounds/"
    property var ubuntuArtList: []
    property var customList: []
    property var activeTransfer
    property var store

    title: homeScreen ? i18n.tr("Home screen") : i18n.tr("Welcome screen")

    Component.onCompleted: {
        ubuntuArtList = backgroundPanel.listUbuntuArt(ubuntuArtDir);
        store = ContentHub.defaultStoreForType(ContentType.Pictures);
        customList = backgroundPanel.listCustomArt(store.uri);
    }

    Action {
        id: selectDefaultPeer
        text: i18n.tr("Photo/Image")
        iconName: "import-image"
        onTriggered: {
            startContentTransfer(function(url) {
                if (gsettings.backgroundDuplicate) {
                    mainPage.updateBoth(url)
                } else {
                    if (homeScreen) {
                        mainPage.updateHome(url);
                        gsettings.backgroundSetLast = "home";
                    } else {
                        mainPage.updateWelcome(url);
                        gsettings.backgroundSetLast = "welcome";
                    }
                }
                pageStack.pop();
            });
        }
    }

    tools: ToolbarItems {
        ToolbarButton {
            action: selectDefaultPeer
        }
    }

    UbuntuBackgroundPanel {
        id: backgroundPanel
    }

    Flickable {
        id: sourceSelector
        anchors.fill: parent
        visible: true
        contentHeight: sourceColumn.height

        Column {
            id: sourceColumn
            anchors.left: parent.left
            anchors.right: parent.right
            height: childrenRect.height
            spacing: units.gu(1)

            WallpaperGrid {
                anchors.left: parent.left
                anchors.right: parent.right
                columns: 2
                model: ubuntuArtList
                title: i18n.tr("Ubuntu Art")
            }

            WallpaperGrid {
                anchors.left: parent.left
                anchors.right: parent.right
                columns: 2
                model: customList
                title: i18n.tr("Custom")
            }
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

    function startContentTransfer(callback) {
        if (callback)
            contentHubConnection.imageCallback = callback
        var transfer = ContentHub.importContent(
                    ContentType.Pictures,
                    ContentHub.defaultSourceForType(ContentType.Pictures));
        if (transfer != null)
        {
            transfer.selectionType = ContentTransfer.Single;
            transfer.setStore(store);
            activeTransfer = transfer;
            activeTransfer.start();
        }
    }
}
