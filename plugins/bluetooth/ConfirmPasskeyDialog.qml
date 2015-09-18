/*
 * This file is part of ubuntu-system-settings
 *
 * Copyright (C) 2013-2015 Canonical Ltd.
 *
 * Contact: Charles Kerr <charles.kerr@canonical.com>
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
import Ubuntu.Components.Popups 1.3

Dialog {
    id: root
    title: i18n.tr("Bluetooth Pairing Request")

    property string name: "Unknown"
    property string passkey: "000000"

    signal canceled
    signal confirmed

    // TRANSLATORS: %1 is the name of the bluetooth device being paired
    text: i18n.tr("Please confirm that the PIN displayed on '%1' matches this one").arg(root.name)

    Label {
        text: root.passkey
        fontSize: "x-large"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Row {
        spacing: units.gu(1)
        Button {
            text: i18n.tr("Cancel")
            onClicked: {
                root.canceled()
                PopupUtils.close(root)
            }
            width: (parent.width - parent.spacing) / 2
        }
        Button {
            text: i18n.tr("Confirm PIN")
            onClicked: {
                root.confirmed()
                PopupUtils.close(root)
            }
            width: (parent.width - parent.spacing) / 2
        }
    }
}
