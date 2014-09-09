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
import QtQuick.Layouts 1.1
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
        return password.length === 5  ||
               password.length === 10 ||
               password.length === 13 ||
               password.length === 26;
    }
    Flickable {
        id: otherNetworkFlickable
        contentWidth: parent.width
        contentHeight: otherview.height + units.gu(8)
        anchors {
            fill: parent
            top: parent.top
            bottom: buttons.top
        }


        Column {
            id : otherview
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            ListItem.Standard {
                text : i18n.tr("Network name")
                showDivider: false
            }

            TextField {
                id : networkname
                width: parent.width - units.gu(4)
                anchors.horizontalCenter: parent.horizontalCenter
            }

            ListItem.ItemSelector {
                id: securityList
                text: i18n.tr("Security")
                model: [i18n.tr("None"),                 // index: 0
                        i18n.tr("WPA & WPA2 Personal"),  // index: 1
                        i18n.tr("WEP"),                  // index: 2
                        ]
            }

            ListItem.Standard {
                id: passwordList
                visible: securityList.selectedIndex !== 0
                text: i18n.tr("Password")
                control : TextInput {
                }
                showDivider: false
            }

            TextField {
                id : password
                visible: securityList.selectedIndex !== 0
                width: parent.width - units.gu(4)
                anchors.horizontalCenter: parent.horizontalCenter
                echoMode: passwordVisibleSwitch.checked ? TextInput.Normal : TextInput.Password
                onActiveFocusChanged: {
                    parent.parent.contentY = 1000
                }
            }

            ListItem.Standard {
                text: i18n.tr("Password visible")
                visible: securityList.selectedIndex !== 0
                id: passwordVisible
                control: Switch {
                    id: passwordVisibleSwitch
                }
            }
        }
    }
    Rectangle {
        id: buttons
        color: Theme.palette.normal.background
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: buttonRow.height + units.gu(4)
        RowLayout {
            id: buttonRow
            anchors {
                margins: units.gu(2)
                verticalCenter: parent.verticalCenter
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(2)
            height: cancelButton.height

            Button {
                id: cancelButton
                Layout.fillWidth: true
                text: i18n.tr("Cancel")
                onClicked: {
                    pageStack.pop()
                }
            }

            Button {
                id: connectButton
                Layout.fillWidth: true
                text: i18n.tr("Connect")
                enabled: settingsValid()
                onClicked: {
                    DbusHelper.connect(networkname.text, securityList.selectedIndex, password.text)
                    pageStack.pop()
                }
            }
        }
    }
}
