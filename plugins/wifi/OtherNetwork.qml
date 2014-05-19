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

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Wifi 1.0
import QMenuModel 0.1

ItemPage {
    id: othernetwork
    title: i18n.tr("Other network")

    Column {
        id : otherview
        anchors.fill: parent

        ListItem.Standard {
            text : i18n.tr("Network name")
            control : TextInput {
              id : networkname
              text : "d"
            }
        }

        ListItem.ItemSelector {
            text: i18n.tr("Security")
            model : [i18n.tr("None"),
                     i18n.tr("WPA Personal")
                     ]
        }
        
        ListItem.Standard {
            text: i18n.tr("Password")
            control : TextInput {
                id : password
                text : "d"
            }
        }

        Item {
            id: buttonRectangle

            anchors.left: parent.left
            anchors.right: parent.right

            Button {
                id: cancelButton

                text: i18n.tr("Cancel")

                anchors.left: parent.left
                anchors.right: parent.horizontalCenter

                onClicked: {
                    pageStack.pop()
                }
            }

            Button {
                id: connectButton

                text: i18n.tr("Connect")
//                enabled: languageList.currentIndex != plugin.currentLanguage

                anchors.left: parent.horizontalCenter
                anchors.right: parent.right

                onClicked: {
                    pageStack.pop() // Fixme, also do something.
                }
            }
        }
    }
}
