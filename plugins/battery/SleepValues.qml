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
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import SystemSettings 1.0
import Ubuntu.SystemSettings.Battery 1.0

ItemPage {
    id: root
    objectName: "sleepValues"
    flickable: scrollWidget

    property alias usePowerd: batteryBackend.powerdRunning
    property bool lockOnSuspend
    property variant idleValues: [30,60,120,180,240,300,600,0]

    UbuntuBatteryPanel {
        id: batteryBackend
    }

    GSettings {
        id: powerSettings
        schema.id: usePowerd ? "com.ubuntu.touch.system" : "org.gnome.desktop.session"
        onChanged: {
            if (key == "activityTimeout" || key == "idleDelay") {
                var curIndex = idleValues.indexOf(value)
                if( curIndex != -1)
                    sleepSelector.selectedIndex = curIndex
            }
	    console.warn("Change on: " + key)
        }
        Component.onCompleted: {
            if (usePowerd)
                sleepSelector.selectedIndex = idleValues.indexOf(powerSettings.activityTimeout)
            else
                sleepSelector.selectedIndex = idleValues.indexOf(powerSettings.idleDelay)
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

            SettingsItemTitle {
                text: lockOnSuspend ? i18n.tr("Lock the device when it's not in use:") :
                                      i18n.tr("Put the device to sleep when it is not in use:")
            }

            ListItem.ItemSelector {
                id: sleepSelector
                objectName: "sleepSelector"
                delegate: OptionSelectorDelegate {
                    text: modelData
                }
                model: [
                    // TRANSLATORS: %1 is the number of seconds
                    i18n.tr("After %1 seconds",
                            "After %1 seconds",
                            30).arg(30),
                    // TRANSLATORS: %1 is the number of minutes
                    i18n.tr("After %1 minute",
                            "After %1 minutes",
                            1).arg(1),
                    // TRANSLATORS: %1 is the number of minutes
                    i18n.tr("After %1 minute",
                            "After %1 minutes",
                            2).arg(2),
                    // TRANSLATORS: %1 is the number of minutes
                    i18n.tr("After %1 minute",
                            "After %1 minutes",
                            3).arg(3),
                    // TRANSLATORS: %1 is the number of minutes
                    i18n.tr("After %1 minute",
                            "After %1 minutes",
                            4).arg(4),
                    // TRANSLATORS: %1 is the number of minutes
                    i18n.tr("After %1 minute",
                            "After %1 minutes",
                            5).arg(5),
                    // TRANSLATORS: %1 is the number of minutes
                    i18n.tr("After %1 minute",
                            "After %1 minutes",
                            10).arg(10),
                    i18n.tr("Never")]
                expanded: true
                onDelegateClicked: {
                  if (usePowerd) {
 		    powerSettings.activityTimeout = idleValues[index]
                    // ensure dimTimeout is 10 seconds less than activityTimeout
                    powerSettings.dimTimeout = Math.max(powerSettings.activityTimeout - 10, 0)
		    console.warn("activity: " +  powerSettings.activityTimeout)
    		    console.warn("dim: " +  powerSettings.dimTimeout)
		  }
                  else
                    powerSettings.idleDelay = idleValues[index]
                }
                highlightWhenPressed: false
            }

            ListItem.Caption {
                text: lockOnSuspend ? i18n.tr("Shorter times are more secure. Device won't lock during calls or video playback.") : i18n.tr("Device won’t sleep during calls or video playback.")
            }
        }
    }
}
