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
import Ubuntu.Components.Popups 1.3

Dialog {
    id: dialogueInstall
    objectName: "imagePrompt"

    property alias havePowerForUpdate: installBtn.visible

    signal requestSystemUpdate()

    title: i18n.tr("Update System")
    text: havePowerForUpdate
            ? i18n.tr("The device needs to restart to install the system update.")
            : i18n.tr("Connect the device to power before installing the system update.")

    Button {
        id: installBtn
        objectName: "imagePromptInstall"
        text: i18n.tr("Restart & Install")
        color: UbuntuColors.orange
        onClicked: {
            dialogueInstall.requestSystemUpdate();
            PopupUtils.close(dialogueInstall);
        }
    }

    Button {
        objectName: "imagePromptCancel"
        text: i18n.tr("Cancel")
        color: UbuntuColors.warmGrey
        onClicked: PopupUtils.close(dialogueInstall)
    }
}
