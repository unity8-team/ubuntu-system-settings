/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Ken VanDine <ken.vandine@canonical.com>
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
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

Column {
    anchors.left: parent.left
    anchors.right: parent.right
    property bool pocketPC: false
    
    ListItem.Caption {
        text: {
            if (pocketPC)
                return i18n.tr("Connect a mouse or touchpad via USB, or use a Bluetooth device. If a Bluetooth device isn’t detected, make sure it is turned on and its batteries are charged.")
            else
                return i18n.tr("You need to use a Bluetooth mouse or touchpad with this display. Make sure it is close to the device and its batteries are charged.")
        }
    }
}
