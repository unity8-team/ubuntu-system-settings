/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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
import Ubuntu.Components 1.3
import SystemSettings 1.0

ItemPage {
    id: appNotificationsPage

    property alias enableNotifications: enableNotificationsChecked.checked
    property alias soundsNotify: soundsChecked.checked
    property alias vibrationsNotify: vibrationsChecked.checked
    property alias bubblesNotify: bubblesChecked.checked
    property alias listNotify: listChecked.checked

    function disableNotificationsWhenAllUnchecked() {
        if (!soundsNotify && !vibrationsNotify && !bubblesNotify && !listNotify) {
            enableNotifications = false
        }
    }

    onSoundsNotifyChanged: {
        if (!soundsNotify) {
            disableNotificationsWhenAllUnchecked()
        }
    }

    onVibrationsNotifyChanged: {
        if (!vibrationsNotify) {
            disableNotificationsWhenAllUnchecked()
        }
    }

    onBubblesNotifyChanged: {
        if (!bubblesNotify) {
            disableNotificationsWhenAllUnchecked()
        }
    }

    onListNotifyChanged: {
        if (!listNotify) {
            disableNotificationsWhenAllUnchecked()
        }
    }

    Column {
        id: notificationsColumn

        anchors.fill: parent

        ListItem {
            height: enableNotificationsLayout.height + (divider.visible ? divider.height : 0)
            ListItemLayout {
                id: enableNotificationsLayout
                title.text: i18n.tr("Enable Notifications")
                CheckBox {
                    id: enableNotificationsChecked
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
        }

        ListItem {
            ListItemLayout { title.text: i18n.tr("Let this app alert me using:") }
        }

        ListItem {
            height: soundsLayout.height + (divider.visible ? divider.height : 0)
            ListItemLayout {
                id: soundsLayout
                title.text: i18n.tr("Sounds")
                CheckBox {
                    id: soundsChecked
                    SlotsLayout.position: SlotsLayout.Leading
                    enabled: appNotificationsPage.enableNotifications
                }
            }
        }

        ListItem {
            height: vibrationsLayout.height + (divider.visible ? divider.height : 0)
            ListItemLayout {
                id: vibrationsLayout
                title.text: i18n.tr("Vibrations")
                CheckBox {
                    id: vibrationsChecked
                    SlotsLayout.position: SlotsLayout.Leading
                    enabled: appNotificationsPage.enableNotifications
                }
            }
        }

        ListItem {
            height: bubblesLayout.height + (divider.visible ? divider.height : 0)
            ListItemLayout {
                id: bubblesLayout
                title.text: i18n.tr("Notification Bubbles")
                CheckBox {
                    id: bubblesChecked
                    SlotsLayout.position: SlotsLayout.Leading
                    enabled: appNotificationsPage.enableNotifications
                }
            }
        }

        ListItem {
            height: listLayout.height + (divider.visible ? divider.height : 0)
            ListItemLayout {
                id: listLayout
                title.text: i18n.tr("Notification List")
                CheckBox {
                    id: listChecked
                    SlotsLayout.position: SlotsLayout.Leading
                    enabled: appNotificationsPage.enableNotifications
                }
            }
        }
    }
}
