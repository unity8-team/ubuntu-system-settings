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

import GSettings 1.0
import QtQuick 2.4
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Components 1.3
import Ubuntu.Settings.Menus 0.1 as Menus

ItemPage {
    id: root
    objectName: "launcherPage"
    title: i18n.tr("Launcher")

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
            Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(1)

            SettingsItemTitle {
                text: i18n.tr("On large screens:")
            }

            SettingsListItems.Standard {
                id: alwaysShowLauncher
                objectName: "alwaysShowLauncher"
                text: i18n.tr("Always")
                layout.subtitle.text: i18n.tr("Videos and full-screen games may hide it temporarily.")

                Switch {
                    id: alwaysShowLauncherSwitch
                    objectName: "alwaysShowLauncherSwitch"
                    checked: unity8Settings.autohideLauncher
                    onTriggered: unity8Settings.autohideLauncher = checked
                }
            }

            Menus.SliderMenu {
                text: i18n.tr("Icon size:")
    
                id: iconWidth
                objectName: "iconWidth"
                function formatValue(v) { return v.toFixed(2) }
                minimumValue: 6
                maximumValue: 12
                value: unity8Settings.launcherWidth
                live: true
            }
        }
    }

    GSettings {
        id: unity8Settings
        schema.id: "com.canonical.Unity8"
    }
}
