/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-14 Canonical Ltd.
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

import GSettings 1.0
import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Brightness 1.0
import Ubuntu.Settings.Components 0.1 as SettingsCompenents

ItemPage {
    id: root
    objectName: "brightnessPage"

    title: i18n.tr("Brightness")

    UbuntuBrightnessPanel {
        id: brightnessPanel
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        BrightnessSlider {}

        ListItem.Standard {
            id: adjust
            text: i18n.tr("Adjust automatically")
            visible: brightnessPanel.powerdRunning &&
                     brightnessPanel.autoBrightnessAvailable
            control: SettingsCompenents.SyncCheckBox {
                id: autoAdjustCheck
                dataTarget: gsettings
                dataProperty: "autoBrightness"
                bidirectional: true
            }
            showDivider: false
        }
        ListItem.Caption {
            text: i18n.tr(
                    "Brightens and dims the display to suit the surroundings.")
            visible: adjust.visible
        }
    }

    GSettings {
        id: gsettings
        schema.id: "com.ubuntu.touch.system"
    }
}
