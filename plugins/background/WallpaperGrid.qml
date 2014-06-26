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
import "utilities.js" as Utilities

Column {
    id: wallpaperGrid

    property var bgmodel
    property int columns
    property int itemWidth: ((mainPage.width - (grid.spacing * (columns - 1))) - (grid.anchors.margins * 2)) / columns
    property int itemHeight: (mainPage.height / mainPage.width) * itemWidth
    property string title

    // path to current background
    property string current

    // whether or not the current background in this model
    property bool holdsCurrent: (bgmodel.indexOf(current) >= 0)

    // can backgrounds be removed
    property bool editable: false

    // the grid a user can populate
    property bool isCustom: false

    // plugin
    property var backgroundPanel

    // signal for when a background was selected
    signal selected (string uri)

    anchors {
        left: parent.left
        right: parent.right
    }

    height: state === "" ? childrenRect.height : header.height
    clip: true
    visible: bgmodel.length > 0 || isCustom
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
            margins: units.gu(2)
        }
        columns: wallpaperGrid.columns
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
                                onTriggered: {

                                    // removing current background, revert to default
                                    if (modelData === current) {
                                        Utilities.revertBackgroundToDefault();
                                    }
                                    backgroundPanel.rmFile(modelData)
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Item {
        width: parent.width
        height: units.gu(2)
        visible: !parent.isCustom
    }

    Row {
        id: customButtons
        visible: parent.isCustom
        spacing: units.gu(2)
        width: parent.width - spacing * 2
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        height: addCustomBgsButton.height + (spacing * 2)
        Button {
            id: addCustomBgsButton
            action: selectDefaultPeer
            objectName: "addCustomBackgroundsButton"
            text: i18n.tr("Add an Image…")
            width: (customButtons.width - parent.spacing) / 2
            anchors {
                verticalCenter: parent.verticalCenter
            }
        }

        Button {
            id: rmCustomBgsButton
            action: deleteCustomBackgrounds
            gradient: UbuntuColors.greyGradient
            objectName: "removeCustomBackgroundsButton"
            text: i18n.tr("Remove Images…")
            width: (customButtons.width - parent.spacing) / 2
            anchors {
                verticalCenter: parent.verticalCenter
            }
        }
    }
}
