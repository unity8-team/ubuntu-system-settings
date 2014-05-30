/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
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
import Ubuntu.SystemSettings.Phone 1.0
import Ubuntu.SystemSettings.Wifi 1.0

ItemPage {

    id: networkDetails
    property string networkName
    property string password
    property string lastUsed
    property string dbusPath

    title: i18n.tr("Network") + " '" + networkName  + "'"
//    anchors.fill: parent.fill

    Column {
        id: lastRectangle

        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.Standard {
            id: lastLabel
            text: i18n.tr("Last connected")

            anchors.left: parent.left
            anchors.right: parent.horizontalCenter

        }

        ListItem.Standard {
            id: lastField

            text: networkDetails.lastUsed

            anchors.left: parent.horizontalCenter
            anchors.right: parent.right

        }
    }

    Column {
        id: passwordRectangle

        anchors.top: lastRectangle.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.Standard {
            id: passwordLabel
            text: i18n.tr("Password")

            anchors.left: parent.left
            anchors.right: parent.horizontalCenter

        }

        ListItem.Standard {
            id: passwordField

            text: networkDetails.password

            anchors.left: parent.horizontalCenter
            anchors.right: parent.right

        }
    }

    Button {
       anchors.top: passwordRectangle.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        text : i18n.tr("Forget network")
        onClicked : {
          DbusHelper.forgetConnection(dbusPath)
          pageStack.pop()
        }
    }
}
