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
import Ubuntu.Components.Popups 0.1
//import Ubuntu.SystemSettings.Background 1.0

Column {
    id: wallpaperGrid
    anchors.left: parent.left
    anchors.right: parent.right
    height: childrenRect.height
    spacing: units.gu(2)

    property var bgmodel
    property int columns
    property int itemWidth: (mainPage.width * 0.7) / columns
    property int itemHeight: (mainPage.height / mainPage.width) * itemWidth
    property string title
    property string current
    property bool editable: false
    property var backgroundPanel
    signal selected (string uri)


    /*
    UbuntuBackgroundPanel {
        id: backgroundPanel
        onCustomBackgroundsChanged: console.log ("onCustomBackgroundsChanged2:" + backgroundPanel.customBackgrounds)
    }
    */

    ListItem.Standard {
        anchors.left: parent.left
        anchors.right: parent.right
        text: title
        showDivider: false
    }

    Grid {
        anchors.horizontalCenter: parent.horizontalCenter
        columns: wallpaperGrid.columns
        spacing: itemWidth * 0.2
        height: childrenRect.height
        Repeater {
            model: bgmodel
            Item {
                width: itemWidth + units.gu(2)
                height: itemHeight + units.gu(2)
                Rectangle {
                    id: itemRect
                    anchors.fill: parent
                    color: UbuntuColors.coolGrey
                    visible: (current === modelData) && (itemImage.status === Image.Ready)
                }
                Image {
                    id: itemImage
                    anchors.centerIn: parent
                    source: modelData
                    width: itemWidth
                    height: itemHeight
                    sourceSize.width: width
                    sourceSize.height: height
                    fillMode: Image.PreserveAspectCrop
                    asynchronous: true
                    ActivityIndicator {
                        anchors.centerIn: parent
                        running: parent.status === Image.Loading
                        visible: running
                    }

                    MouseArea {
                        anchors.fill: parent
                        onPressAndHold: {
                            console.log ("onPressAndHold: " + modelData);
                            if (editable) {
                                actPop.target = itemImage;
                                actPop.show();
                            }
                        }
                        onClicked: {
                            if (!actPop.visible)
                                selected(modelData);
                        }
                    }
                }
                ActionSelectionPopover {
                    id: actPop
                    target: parent
                    pointerTarget: parent
                    dismissArea: parent
                    delegate: ListItem.Standard {
                        text: action.text
                    }
                    actions: ActionList {
                        Action {
                            text: i18n.tr("Remove")
                            onTriggered: {
                                print(text);
                                print(modelData);
                                backgroundPanel.rmFile(modelData);
                                actPop.hide();
                            }
                        }
                    }
                }
            }
        }
    }
}
