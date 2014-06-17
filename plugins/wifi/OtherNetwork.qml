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

    function settingsValid() {
        if(networkname.length == 0) {
            return false;
        }
        if(securityList.selectedIndex == 0) {
            return true
        }
        if(securityList.selectedIndex == 1) {
            return password.length >= 8
        }
        // WEP
        return password.length==5 || password.length == 10;
    }


    Column {
        id : otherview
        anchors.fill: parent

        ListItem.Standard {
            text : i18n.tr("Network name")
        }

        TextField {
            id : networkname
            width: parent.width
        }

        ListItem.ItemSelector {
            id: securityList
            text: i18n.tr("Security")
            model: [i18n.tr("None"),
                    i18n.tr("WPA & WPA2 Personal"),
                    i18n.tr("WEP"),
                    ]
        }

        ListItem.Standard {
            id: passwordList
            text: i18n.tr("Password")
            control : TextInput {
            }
        }

        TextField {
            id : password
            width: parent.width
        }

        Item {
            id: buttonRectangle

            anchors.top: password.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: units.gu(1)

            Button {
                id: cancelButton
                anchors.leftMargin: units.gu(2)
                anchors.rightMargin: units.gu(1)
                anchors.bottomMargin: units.gu(1)

                text: i18n.tr("Cancel")

                anchors.left: parent.left
                anchors.right: parent.horizontalCenter

                onClicked: {
                    pageStack.pop()
                }
            }

            Button {
                id: connectButton

                anchors.leftMargin: units.gu(1)
                anchors.rightMargin: units.gu(2)
                anchors.bottomMargin: units.gu(1)
                text: i18n.tr("Connect")
//                enabled: languageList.currentIndex != plugin.currentLanguage

                anchors.left: parent.horizontalCenter
                anchors.right: parent.right

                enabled: settingsValid()

                onClicked: {
                    DbusHelper.connect(networkname.text, securityList.currentindex, password.text)
                    pageStack.pop()
                }
            }
        }
    }
}
