/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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

import QtQuick 2.4
import QtQuick.Layouts 1.1
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.Wifi 1.0

ItemPage {

    id: networkDetails
    objectName: "networkDetailsPage"

    property string networkName
    property string password
    property string lastUsed
    property string dbusPath

    title: i18n.tr("Network details")

    Flickable {
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > networkDetails.height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
        /* Set the direction to workaround
           https://bugreports.qt-project.org/browse/QTBUG-31905 otherwise the UI
           might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {

            anchors.left: parent.left
            anchors.right: parent.right

            SettingsListItems.SingleValue {
                text: i18n.tr("Name")
                value: networkName
            }

            SettingsListItems.SingleValue {
                id: lastLabel
                text: i18n.tr("Last connected")
                value: networkDetails.lastUsed.length !== 0 ?
                    networkDetails.lastUsed : i18n.tr("Never")
            }

            SettingsListItems.Standard {
                id: passwordLabel
                text: i18n.tr("Password")
                visible: networkDetails.password.length !== 0

                TextInput {
                    id: passwordField
                    readOnly: true
                    text: networkDetails.password
                    echoMode: passwordVisibleSwitch.checked ?
                        TextInput.Normal : TextInput.Password
                }
            }

            SettingsListItems.Standard {
                id: passwordVisible
                text: i18n.tr("Show password")
                visible: networkDetails.password.length !== 0

                Switch {
                    id: passwordVisibleSwitch
                }
            }

            SettingsListItems.SingleControl {

                Button {
                    objectName: "forgetNetwork"
                    text : i18n.tr("Forget this network")
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: units.gu(2)
                    }
                    onClicked : {
                        DbusHelper.forgetConnection(dbusPath);
                        pageStack.pop();
                    }
                }
            }
        }
    }
}
