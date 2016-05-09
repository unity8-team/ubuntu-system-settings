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
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.SystemSettings.Notifications 1.0
import SystemSettings 1.0

ItemPage {
    property ClickApplicationEntry entry;

    title: entry.displayName

    ListItems.Base {
        id: subtitle
        height: labelSubtitle.height + units.gu(2)

        Label {
            id: labelSubtitle
            text: i18n.tr("Let this app alert me using:")
            wrapMode: Text.WordWrap
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: units.gu(1)
            }
        }

        highlightWhenPressed: false
    }

    Column {
        id: notificationsColumn

        anchors {
            top: subtitle.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        ListItem {
            height: soundsLayout.height + (divider.visible ? divider.height : 0)
            ListItemLayout {
                id: soundsLayout
                title.text: i18n.tr("Sounds")
                CheckBox {
                    checked: entry.soundsNotify
                    SlotsLayout.position: SlotsLayout.Leading
                    onCheckedChanged: entry.soundsNotify = checked
                }
            }
        }

        ListItem {
            height: vibrationsLayout.height + (divider.visible ? divider.height : 0)
            ListItemLayout {
                id: vibrationsLayout
                title.text: i18n.tr("Vibrations")
                CheckBox {
                    checked: entry.vibrationsNotify
                    SlotsLayout.position: SlotsLayout.Leading
                    onCheckedChanged: entry.vibrationsNotify = checked
                }
            }
        }

        ListItem {
            height: bubblesLayout.height + (divider.visible ? divider.height : 0)
            ListItemLayout {
                id: bubblesLayout
                title.text: i18n.tr("Notification Bubbles")
                CheckBox {
                    checked: entry.bubblesNotify
                    SlotsLayout.position: SlotsLayout.Leading
                    onCheckedChanged: entry.bubblesNotify = checked
                }
            }
        }

        ListItem {
            height: listLayout.height + (divider.visible ? divider.height : 0)
            ListItemLayout {
                id: listLayout
                title.text: i18n.tr("Notification List")
                CheckBox {
                    checked: entry.listNotify
                    SlotsLayout.position: SlotsLayout.Leading
                    onCheckedChanged: entry.listNotify = checked
                }
            }
        }
    }
}
