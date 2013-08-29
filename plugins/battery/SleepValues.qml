/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Sebastien Bacher <sebastien.bacher@canonical.com>
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

// THIS FILE IS LOADED DIRECTLY BY NAME FROM THE SECURITY-PRIVACY PANEL.
// IF YOU RENAME IT YOU MUST UPDATE THE REFERENCE THERE.

import GSettings 1.0
import QtQuick 2.0
import QtSystemInfo 5.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import SystemSettings 1.0
import Ubuntu.SystemSettings.Battery 1.0

ItemPage {
    id: root

    flickable: scrollWidget

    property alias usePowerd: batteryBackend.powerdRunning
    property bool lockOnSuspend

    UbuntuBatteryPanel {
        id: batteryBackend
    }

    GSettings {
        id: powerSettings
        schema.id: usePowerd ? "com.canonical.powerd" : "org.gnome.settings-daemon.plugins.power"
        onChanged: {
            if([60,120,180,240,300].indexOf(value) != -1)
                if (key == "activityTimeout" || key == "sleepDisplayBattery")
                    sleepSelector.selectedIndex = (value/60)-1
        }
        Component.onCompleted: {
            if (usePowerd)
                sleepSelector.selectedIndex = (powerSettings.activityTimeout === 0) ? 5 : powerSettings.activityTimeout/60-1
            else
                sleepSelector.selectedIndex = (powerSettings.sleepDisplayBattery === 0) ? 5 : powerSettings.sleepDisplayBattery/60-1
        }
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Standard {
                text: lockOnSuspend ? i18n.tr("Lock the phone when it's not in use:") : i18n.tr("Put the phone to sleep when it is not in use:")
            }

            ListItem.ValueSelector {
                id: sleepSelector
                expanded: true
                // TODO: There is no way to have a ValueSelector always expanded
                onExpandedChanged: expanded = true
                values: [
                    i18n.tr("After %1 minute".arg(1),
                            "After %1 minutes".arg(1),
                            1),
                    i18n.tr("After %1 minute".arg(2),
                            "After %1 minutes".arg(2),
                            2),
                    i18n.tr("After %1 minute".arg(3),
                            "After %1 minutes".arg(3),
                            3),
                    i18n.tr("After %1 minute".arg(4),
                            "After %1 minutes".arg(4),
                            4),
                    i18n.tr("After %1 minute".arg(5),
                            "After %1 minutes".arg(5),
                            5),
                    i18n.tr("Never")]
                onSelectedIndexChanged: {
                  if (usePowerd)
                    powerSettings.activityTimeout = (selectedIndex == 5) ? 0 : (selectedIndex+1)*60
                  else
                    powerSettings.sleepDisplayBattery = (selectedIndex == 5) ? 0 : (selectedIndex+1)*60
                }
            }

            ListItem.Caption {
                text: lockOnSuspend ? i18n.tr("Shorter times are more secure. Phone won't lock during calls or video playback.") : i18n.tr("Phone won’t sleep during calls or video playback.")
            }
        }
    }
}
