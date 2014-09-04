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
import "Components"
import "utilities.js" as Utilities

Column {
    id: wallpaperGrid

    property var bgmodel
    property int columns

    // actual grid width, that is the main page's width minus spacing and margins
    property int gridWidth: mainPage.width
                            // subtract spacing
                            - ((columns - 1) * grid.spacing)
                            // subtract margins
                            - (grid.anchors.leftMargin + grid.anchors.rightMargin)

    property int itemWidth: gridWidth / columns
    property int itemHeight: (mainPage.height / gridWidth) * itemWidth

    property string title

    // path to current background
    property string current

    // whether or not the current background in this model
    property bool holdsCurrent: (bgmodel.indexOf(current) >= 0)

    // can backgrounds be removed
    property bool editable: false

    // user can add/remove backgrounds
    property bool isCustom: false

    // plugin
    property var backgroundPanel

    // signal for when a background was selected
    signal selected (string uri)

    anchors {
        left: parent.left
        right: parent.right
    }

    // if collapsed, reduce height to that of the header
    height: state === "" ? childrenRect.height : header.height
    clip: true
    visible: bgmodel.length > 0 || isCustom
    state: holdsCurrent ? "" : "collapsed"
    states: [
        State {
            name: "collapsed"
            PropertyChanges {
                target: grid
                visible: false
            }
        }
    ]

    ListItemsHeader {
        id: header
        objectName: title.toString() + "Header"
        anchors.left: parent.left
        anchors.right: parent.right
        text: title
        enabled: !holdsCurrent
        image: {
            if (parent.holdsCurrent) {
                return "bullet.png"
            }
            return parent.state === "collapsed" ? "header_handlearrow.png" : "header_handlearrow2.png"
        }
        onClicked: {
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
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        columns: wallpaperGrid.columns
        height: childrenRect.height
        spacing: units.gu(2)
        visible: parent.state === ""
        states: [
            State {
                name: ""
                StateChangeScript {
                    name: "deSelectBackgrounds"
                    script: Utilities.deSelectBackgrounds(gridRepeater);
                }
            },
            State {
                name: "selection"
            }
        ]
        Repeater {
            id: gridRepeater
            objectName: "gridRepeater"
            model: bgmodel
            Item {
                width: itemWidth
                height: itemHeight
                id: gridItem
                states: [
                    State {
                        name: "selected"
                        PropertyChanges {
                            target: selectionTick
                            visible: true
                        }
                    }
                ]
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width
                    height: parent.height
                    Image {
                        property bool current: current === modelData
                        id: itemImage
                        objectName: "itemImg"
                        source: modelData
                        width: parent.width
                        height: parent.height
                        sourceSize.width: 512
                        fillMode: Image.PreserveAspectCrop
                        asynchronous: true
                        smooth: true
                    }
                    HighlightedOverlay {
                        id: highLight
                        objectName: "highLight"
                        visible: (current === modelData) && (itemImage.status === Image.Ready)
                    }
                    SelectedOverlay {
                        id: selectionTick
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
                    ActionSelectionPopover {
                        z: 100
                        id: actPop
                        caller: emptyItemForCaller
                        delegate: ListItem.Standard {
                            text: action.text
                        }
                        actions: ActionList {
                            Action {
                                text: i18n.tr("Remove")
                                onTriggered: {
                                    // removing current background, revert to default
                                    if (modelData === current) {
                                        Utilities.revertBackgroundToDefault();
                                    }
                                    backgroundPanel.rmFile(modelData);
                                }
                            }
                        }
                    }
                    MouseArea {
                        id: imgMouseArea
                        anchors.fill: parent
                        onPressAndHold: {
                            if (editable && (grid.state === "")) {
                                actPop.show();
                            }
                        }
                        onClicked: {
                            if (!actPop.visible && (grid.state === "")) {
                                selected(modelData);
                            }

                            if (grid.state === "selection") {
                                gridItem.state = gridItem.state === "" ? "selected" : "";
                            }
                        }
                    }
                }
            }
        }
    }

    // add some spacing
    Item {
        width: parent.width
        height: units.gu(2)
        visible: !parent.isCustom
    }

    AddRemove {
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        visible: parent.isCustom
        spacing: units.gu(2)
        width: parent.width - spacing * 2
        height: children[0].height + (spacing * 2)
        buttonWidth: (width - spacing) / 2
        repeater: gridRepeater
        onEnteredQueueMode: {
            grid.state = "selection"
        }
        onLeftQueueMode: {
            grid.state = ""
        }
        onRemoveQueued: {
            removeBackgrounds.trigger();
            grid.state = ""
        }
    }

    // Action for removing backgrounds
    Action {
        id: removeBackgrounds
        onTriggered: {
            var toDelete = [];
            // select backgrounds to remove
            for (var i=0, j=gridRepeater.count; i < j; i++) {
                if (gridRepeater.itemAt(i).state === "selected") {
                    toDelete.push(bgmodel[i]);
                }
            }
            // remove backgrounds
            toDelete.forEach(function (bg) {
                if (bg === current) {
                    Utilities.revertBackgroundToDefault();
                }
                backgroundPanel.rmFile(bg);
            });
        }
    }

}
