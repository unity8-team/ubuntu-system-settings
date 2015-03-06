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
import Ubuntu.Components.Popups 0.1


Component {
    id: hotspotSetup

    Dialog {
        id: hotspotSetupDialog
        property var hotspotManager: null
        objectName: "hotspotSetup"
        title: hotspotManager.stored ?
            i18n.tr("Change hotspot setup") : i18n.tr("Setup hotspot")

        Label {
            id: ssidLabel
            text: i18n.tr("Hotspot name")
            fontSize: "medium"
            font.bold: true
            color: Theme.palette.selected.backgroundText
            elide: Text.ElideRight
        }

        TextField {
            id: ssidField
            objectName: "ssidField"
            text: hotspotManager.ssid
        }

        Label {
            id: passwordLabel
            text: i18n.tr("Key (must be 8 characters or longer)")
            fontSize: "medium"
            font.bold: true
            color: Theme.palette.selected.backgroundText
            elide: Text.ElideRight
        }

        TextField {
            id: passwordField
            objectName: "passwordField"
            text: hotspotManager.password
            echoMode: passwordVisibleSwitch.checked ?
                TextInput.Normal : TextInput.Password
        }

        ListItem.Standard {
            text: i18n.tr("Show key")
            id: passwordVisible
            control: Switch {
                id: passwordVisibleSwitch
            }
        }

        RowLayout {

            anchors.margins: units.gu(2)

            Button {
                id: cancelButton
                Layout.fillWidth: true
                text: i18n.tr("Cancel")
                onClicked: PopupUtils.close(hotspotSetupDialog)
            }

            Button {
                id: enableButton
                objectName: "enableButton"
                visible: !hotspotManager.stored
                Layout.fillWidth: true
                text:  i18n.tr("Enable")
                enabled: ssidField.text != "" && passwordField.length >= 8
                onClicked: {
                    hotspotManager.ssid = ssidField.text;
                    hotspotManager.password = passwordField.text;
                    hotspotManager.enabled = true;
                    // hotspotManager.storedChanged.connect(function (stored) {
                    //     console.warn('qml saw storedChanged');
                    //     if (stored) {
                    //         PopupUtils.close(hotspotSetupDialog);
                    //     } else {
                    //         console.warn('qml saw nack of storedChanged');
                    //     }
                    // });
                }
            }

            Button {
                id: changeButton
                visible: hotspotManager.stored
                Layout.fillWidth: true
                text: i18n.tr("Change")
                enabled: enableButton.enabled
                onClicked: {
                    hotspotManager.ssid = ssidField.text;
                    hotspotManager.password = passwordField.text;
                    PopupUtils.close(hotspotSetupDialog);
                }
            }
        }
    }
}
