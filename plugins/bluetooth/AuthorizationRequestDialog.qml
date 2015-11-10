/*
 * This file is part of ubuntu-system-settings
 *
 * Copyright (C) 2013-2015 Canonical Ltd.
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
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog {
    id: root
    title: i18n.tr("Bluetooth Pairing Authorization Request")

    property string name: "Unknown"

    signal accepted
    signal declined

    // TRANSLATORS: %1 is the name of the bluetooth device which requires authorization
    text: i18n.tr("The device %1 wants to pair with this device. Do you want to allow this?").arg(root.name)

    Row {
        spacing: units.gu(1)
        Button {
            text: i18n.tr("Allow")
            onClicked: {
                root.allowed()
                PopupUtils.close(root)
            }
        }
        Button {
            text: i18n.tr("Refuse")
            onClicked: {
                root.denied()
                PopupUtils.close(root)
            }
        }
    }
}
