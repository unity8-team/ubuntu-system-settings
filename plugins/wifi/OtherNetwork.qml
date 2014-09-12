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
import Ubuntu.Components.Popups 0.1
import Ubuntu.SystemSettings.Wifi 1.0
import QMenuModel 0.1

Component {

    Dialog {
        id: otherNetworkDialog
        objectName: "otherNetworkDialog"
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
        title: i18n.tr("Connect to Hidden Network")

        ListItem.Standard {
            text : i18n.tr("Network name")
            showDivider: false
            highlightWhenPressed: false
        }

        TextField {
            id : networkname
            objectName: "networkname"
            inputMethodHints: Qt.ImhNoPredictiveText
        }

        ListItem.ItemSelector {
            id: securityList
            objectName: "securityList"
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
            objectName: "password"
            visible: securityList.selectedIndex !== 0
            echoMode: passwordVisibleSwitch.checked ? TextInput.Normal : TextInput.Password
        }

        ListItem.Standard {
            text: i18n.tr("Password visible")
            visible: securityList.selectedIndex !== 0
            id: passwordVisible
            control: Switch {
                id: passwordVisibleSwitch
            }
        }

        RowLayout {
            id: buttonRow
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(2)
            height: cancelButton.height

            Button {
                id: cancelButton
                objectName: "cancel"
                Layout.fillWidth: true
                text: i18n.tr("Cancel")
                onClicked: {
                    PopupUtils.close(otherNetworkDialog)
                }
            }

            Button {
                id: connectButton
                objectName: "connect"
                Layout.fillWidth: true
                text: i18n.tr("Connect")
                enabled: settingsValid()
                onClicked: {
                    DbusHelper.connect(networkname.text, securityList.selectedIndex, password.text)
                    PopupUtils.close(otherNetworkDialog)
                }
            }
        }
    }

}

