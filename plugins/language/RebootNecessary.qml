/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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
import Ubuntu.Components.Popups 0.1

Dialog {
    id: dialog

    signal reboot()

    text: i18n.tr("The phone needs to restart for changes to take effect.")
    objectName: "rebootNecessaryDialog"
    Button {
        id: reboot
        text: i18n.tr("Restart")
        onClicked: {
            dialog.reboot();
            PopupUtils.close(dialog)
        }
    }
    Button {
        id: dismiss
        text: i18n.tr("Restart later")
        onClicked: {
            PopupUtils.close(dialog)
        }
    }
}
