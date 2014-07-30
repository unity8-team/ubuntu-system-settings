/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2014 Canonical Ltd.
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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Notifications 1.0
import SystemSettings 1.0

ItemPage {
    id: root
    objectName: "systemNotificationsPage"

    title: i18n.tr("Notifications")

    NotificationsManager {
        id: notificationsManager
    }

    Label {
        id: labelSubtitle
        text: i18n.tr("Selected apps can alert you using notification bubbles, sounds, vibrations, and the Notification Center.")
        wrapMode: Text.WordWrap
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }
        fontSize: "large"
    }

    ListItem.ThinDivider {
        id: divider
        anchors {
            top: labelSubtitle.bottom
            left: parent.left
            right: parent.right
            topMargin: units.gu(2)
        }
    }

    ListView {
        id: notificationsList
        objectName: "notificationsList"
        anchors {
            left: parent.left
            right: parent.right
            top: divider.bottom
            bottom: parent.bottom
            margins: units.gu(2)
        }
        model: notificationsManager.model
        clip: true
        contentHeight: contentItem.childrenRect.height

        delegate: ListItem.Standard {
            text: modelData.title
            iconSource: modelData.icon
            iconFrame: false
            control: Switch {
                checked: modelData.status

                onCheckedChanged: {
                    modelData.status = checked;
                }
            }
        }
    }
}
