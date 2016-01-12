/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.SystemSettings.Wifi 1.0

ItemPage {
    id: wifiDisplays
    objectName: "wifiDisplays"
    title: i18n.tr("Wi-Fi Displays")

    Flickable {
        id: pageFlickable
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height

        Column {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            ListItem.Standard {
                objectName: "wifiDisplays"
                text: i18n.tr("Wifi Displays")
                control: Item {
                    Button {}

                }
            }

            Repeater {
                id: mainMenu
                model: menuStack.tail ? menuStack.tail : null
                delegate: Item {
                    id: displayDelegate
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                }
            }
        }

        // Only allow flicking if the content doesn't fit on the page
        boundsBehavior: (contentHeight > wifiDisplays.height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
    }
}
