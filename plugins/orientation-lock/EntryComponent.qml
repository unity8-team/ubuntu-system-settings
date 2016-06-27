/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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
import QtQuick.Window 2.1
import Ubuntu.Components 1.3


ListItem {
    id: root
    height: layout.height

    ListItemLayout {
        id: layout
        title.text: i18n.tr(model.displayName)
        Switch {
            id: control
            objectName: "orientationLockSwitch"

            SlotsLayout.position: SlotsLayout.Trailing
            property bool serverChecked: systemSettings.rotationLock
            onServerCheckedChanged: checked = serverChecked
            Component.onCompleted: checked = serverChecked
            onTriggered: systemSettings.rotationLock = checked

            GSettings {
                id: systemSettings
                schema.id: "com.ubuntu.touch.system"
            }
        }
        Icon {
            SlotsLayout.position: SlotsLayout.Leading;
            SlotsLayout.padding { top: 0; bottom: 0 }
            source: model.icon
            height: units.gu(5)
        }
    }
}
