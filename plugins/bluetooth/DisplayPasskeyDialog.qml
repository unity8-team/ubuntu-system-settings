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


import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog {
    id: root
    title: i18n.tr("Bluetooth Pairing Request")

    property string name: "Unknown"
    property string passkey: "000000"
    property string entered: "0"

    signal canceled

    // TRANSLATORS: %1 is the name of the bluetooth device being paired
    text: i18n.tr("Please enter the following PIN on %1 and press “Enter” on the keyboard:").arg(root.name)

    Label {
        /* display the number of chars that remain to be typed */
        /* TODO: workaround bluez bug #1421598, if the number of entered digit
           doesn't make sense then just display the passkey without masking
           chars as you type */
        text: (entered <= 6) ? root.passkey.slice(entered)+"⏎" : root.passkey+"⏎"
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
            width: (parent.width - parent.spacing)
        }
    }
}
