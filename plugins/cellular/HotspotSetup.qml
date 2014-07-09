/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
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
import QtQuick.Layouts 1.1
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Cellular 1.0

ItemPage {

    id: hotspotSetup
    title: i18n.tr("Change hotspot setup")

    property var hotspotManager: null


    Column {

        anchors.fill: parent

        ListItem.Standard {
            id: ssidLabel
            text: i18n.tr("Hotspot name")
        }

        TextField {
            id: ssidField
            text: hotspotManager.getHotspotName()
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: units.gu(2)
            anchors.rightMargin: units.gu(2)
        }

        ListItem.Standard {
            id: passwordLabel
            text: i18n.tr("Key (must be 8 characters or longer)")
        }

        TextField {
            id: passwordField
            text: hotspotManager.getHotspotPassword()
            echoMode: passwordVisibleSwitch.checked ? TextInput.Normal : TextInput.Password
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: units.gu(2)
            anchors.rightMargin: units.gu(2)
        }

        ListItem.Standard {
            text: i18n.tr("Show key")
            id: passwordVisible
            control: Switch {
                id: passwordVisibleSwitch
            }
        }

        RowLayout {
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(2)
            }
            
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
            text: i18n.tr("Change")
            enabled: ssidField.text != "" && passwordField.length >= 8
            onClicked: {
                hotspotManager.setupHotspot(ssidField.text, passwordField.text)
                pageStack.pop()
            }
        }
        }
    }
}
