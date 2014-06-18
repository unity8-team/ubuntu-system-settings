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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1

Column {
    id: wallpaperGrid

    property var bgmodel
    property int columns
    property int itemWidth: ((mainPage.width - (grid.spacing * (columns - 1))) - (grid.anchors.margins * 2)) / columns
    property int itemHeight: (mainPage.height / mainPage.width) * itemWidth
    property string title
    property string current
    property bool holdsCurrent: (bgmodel.indexOf(current) >= 0)
    property bool editable: false
    property var backgroundPanel
    signal selected (string uri)

    anchors {
        left: parent.left
        right: parent.right
    }

    height: childrenRect.height
    spacing: units.gu(1)

    visible: bgmodel.length > 0

    state: holdsCurrent ? "" : "collapsed"
    states: [
        State {
            name: "collapsed"
            PropertyChanges {
                target: grid
                height: 0
            }
        }
    ]

    ListItemsHeader {
        id: header
        anchors.left: parent.left
        anchors.right: parent.right
        text: title
        image: {
            if (parent.holdsCurrent) {
                return "bullet.png"
            }
            return parent.state === "collapsed" ? "header_handlearrow.png" : "header_handlearrow2.png"
        }
        onClicked: {
            // if current grid holds current image, do nothing
            if (holdsCurrent) {
                return
            }

            if (parent.state === "collapsed") {
                parent.state = ""
            }
            else {
                parent.state = "collapsed"
            }
        }
    }

    Grid {
        id: grid
        anchors {
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }
        columns: wallpaperGrid.columns
        spacing: units.dp(1)
        height: childrenRect.height
        clip: true

        Repeater {
            objectName: "gridRepeater"
            model: bgmodel
            Item {
                width: itemWidth
                height: itemHeight
                UbuntuShape {
                    id: itemBorder
                    anchors.fill: parent
                    color: UbuntuColors.orange
                    radius: "medium"
                    visible: (current === modelData) && (itemImage.status === Image.Ready)
                    objectName: "SelectedShape"
                }
                UbuntuShape {
                    anchors.centerIn: parent
                    anchors.margins: units.dp(5)
                    width: itemWidth - (anchors.margins * 2)
                    height: itemHeight  - (anchors.margins * 2)
                    radius: itemBorder.radius
                    image: Image {
                        property bool current: current === modelData
                        id: itemImage
                        objectName: "itemImg"
                        source: modelData
                        width: itemWidth - (anchors.margins * 2)
                        height: itemHeight  - (anchors.margins * 2)
                        sourceSize.width: 512
                        fillMode: Image.PreserveAspectCrop
                        asynchronous: true
                        smooth: true
                    }
                    ActivityIndicator {
                        anchors.centerIn: parent
                        running: parent.status === Image.Loading
                        visible: running
                    }
                    /* create an empty item centered in the image to align the popover to */
                    Item {
                        id: emptyItemForCaller
                        anchors.centerIn: parent
                    }
                    MouseArea {
                        anchors.fill: parent
                        onPressAndHold: {
                            if (editable)
                                actPop.show();
                        }
                        onClicked: {
                            if (!actPop.visible)
                                selected(modelData);
                        }
                    }
                    ActionSelectionPopover {
                        id: actPop
                        caller: emptyItemForCaller
                        delegate: ListItem.Standard {
                            text: action.text
                        }
                        actions: ActionList {
                            Action {
                                text: i18n.tr("Remove")
                                onTriggered: backgroundPanel.rmFile(modelData)
                            }
                        }
                    }
                }
            }
        }
    }
}
