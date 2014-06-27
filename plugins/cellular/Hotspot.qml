/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Cellular 1.0

ItemPage {

    id: hotspot

    title: i18n.tr("Wi-Fi hotspot")

    Column {

        anchors.fill: parent

        Label {
            width: parent.width
            //autoSize: true
            //readOnly: true
            wrapMode: Text.WordWrap
            anchors.margins: units.gu(2)
            text : i18n.tr("A Wi-Fi hotspot allows other devices to use your cellular data connection. Normal data charges apply.") 
        }

        ListItem.Standard {
            text: i18n.tr("Hotspot")
            control: Switch {
                id: hotspotSwitch
                checked: DbusHelper.isHotspotActive()
                enabled: ssidField.text != "" && passwordField.length >= 8
                onTriggered: {
                    if(checked) {
                        DbusHelper.setupHotspot(ssidField.text, passwordField.text)
                    } else {
                        DbusHelper.disableHotspot(checked)
                    }
                }
            }
        }

        ListItem.Standard {
            id: ssidLabel
            text: i18n.tr("Network name")
        }
        
        TextField {
            id: ssidField
            text: DbusHelper.getHotspotName()
            readOnly: hotspotSwitch.checked
            anchors.left: parent.left
            anchors.right: parent.right
        }

        ListItem.Standard {
            id: passwordLabel
            text: i18n.tr("WPA password")
        }
        
        TextField {
            id: passwordField
            text: DbusHelper.getHotspotPassword()
            echoMode: passwordVisibleSwitch.checked ? TextInput.Normal : TextInput.Password
            readOnly: hotspotSwitch.checked
            anchors.left: parent.left
            anchors.right: parent.right
        }

        ListItem.Standard {
            text: i18n.tr("Show key")
            id: passwordVisible
            control: Switch {
                id: passwordVisibleSwitch
            }
        }

    }
}
