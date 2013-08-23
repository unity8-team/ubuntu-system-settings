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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import SystemSettings 1.0

ItemPage {
    title: i18n.tr("Lock security")
    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.Standard {
            text: i18n.tr("Unlock the phone using:")
        }

        ListItem.ValueSelector {
            id: unlockMethod
            values: [
                i18n.tr("Swipe (no security)"),
                i18n.tr("4-digit passcode"),
                i18n.tr("Passphrase…")
            ]
            expanded: true
            onExpandedChanged: expanded = true
            enabled: false /* TODO: enable when there is a backend */
        }
        ListItem.SingleControl {
            control: Button {
                text: i18n.tr("Change passcode…")
                width: parent.width - units.gu(4)
                enabled: false //unlockMethod.selectedIndex == 1 //passcode
            }
        }
    }
}
