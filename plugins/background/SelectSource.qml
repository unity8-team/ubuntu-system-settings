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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Content 0.1
import SystemSettings 1.0
import Ubuntu.SystemSettings.Background 1.0

import "utilities.js" as Utilities

ItemPage {
    id: selectSourcePage
    flickable: sourceSelector
    anchors.fill: parent


    property int cols: 2
    property int itemWidth: (parent.width * 0.7) / cols
    property int space: itemWidth * 0.3
    property list<ContentPeer> peers

    Component.onCompleted: {
        peers = ContentHub.knownSourcesForType(ContentType.Pictures);
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

            ListItem.Header {
                id: ubuntuArtHeader
                anchors.left: parent.left
                anchors.right: parent.right
                text: i18n.tr("Ubuntu Art")
            }

            Grid {
                id: itemGrid
                anchors.horizontalCenter: parent.horizontalCenter
                columns: cols
                spacing: space
                height: childrenRect.height
                Repeater {
                    model: 20 // FIXME: Populate this with ubuntu art images
                    Item {
                        width: itemWidth
                        height: width
                        UbuntuShape {
                            height: parent.height
                            width: parent.width
                            image: Image {
                                source: "file:///usr/share/backgrounds/Speaker_Weave_by_Phil_Jackson.jpg"
                                width: parent.width
                                height: parent.height
                                fillMode: Image.PreserveAspectFit
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    // FIXME: set background image
                                    console.log ("clicked");
                                }
                            }
                        }
                    }
                }
            }

            ListItem.ThinDivider {}

            ListItem.Header {
                anchors.left: parent.left
                anchors.right: parent.right
                text: i18n.tr("Custom")
            }

            Grid {
                id: customGrid
                anchors.horizontalCenter: parent.horizontalCenter
                columns: cols
                spacing: space
                height: childrenRect.height
                Repeater {
                    model: peers
                    Column {
                        width: itemWidth
                        height: childrenRect.height
                        UbuntuShape {
                            width: parent.width
                            height: width
                            image: Image {
                                // FIXME: replace with app icon
                                source: "file:///usr/share/backgrounds/Speaker_Weave_by_Phil_Jackson.jpg"
                                width: parent.width
                                height: parent.height
                                fillMode: Image.PreserveAspectFit
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    // FIXME: Create import request from content-hub for selected peer
                                    console.log ("clicked");
                                }
                            }
                        }
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: modelData.name
                        }
                    }
                }
            }
        }

    }
}
