/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Cellular 1.0

ItemPage {

    id: hotspot

    title: i18n.tr("Wi-Fi hotspot")

    HotspotManager {
        id: hotspotManager
    }

    Column {

        anchors.fill: parent

        ListItem.Standard {
            text: i18n.tr("Hotspot")
            control: Switch {
                id: hotspotSwitch
                checked: hotspotManager.isHotspotActive()
                onTriggered: {
                    if(checked) {
                        hotspotManager.enableHotspot()
                    } else {
                        hotspotManager.disableHotspot()
                    }
                }
            }
        }

        Label {
            width: parent.width
            wrapMode: Text.WordWrap
            anchors.leftMargin: units.gu(2)
            anchors.rightMargin: units.gu(2)
            text : hotspotSwitch.enabled ?
              i18n.tr("When hotspot is on, other devices can user your cellular data connection over Wi-Fi. Normal data charges apply.")
              : i18n.tr("Other devices can user your cellular data connection over the Wi-Fi network. Normal data charges apply.") 
        }

        Button {
            text: i18n.tr("Set up hotspot")
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: units.gu(2)
            anchors.rightMargin: units.gu(2)
            onClicked: {
                pageStack.push(Qt.resolvedUrl("HotspotSetup.qml"), {hotspotManager: hotspotManager})
            }
        }

    }
}
