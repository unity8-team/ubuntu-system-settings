/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Sebastien Bacher <sebastien.bacher@canonical.com>
 *
 */

import GSettings 1.0
import QMenuModel 0.1
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import SystemSettings 1.0

ItemPage {
    id: dashPage
    title: i18n.tr("Location")

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        QDBusActionGroup {
            id: locationActionGroup
            busType: DBus.SessionBus
            busName: "com.canonical.indicator.location"
            objectPath: "/com/canonical/indicator/location"

            property variant enabled: action("location-detection-enabled")

            Component.onCompleted: start()
        }

        ListItem.Standard {
            text: i18n.tr("Location detection")
            control: Switch {
                id: locationOn
                onClicked: locationActionGroup.enabled.activate()
            }
            visible: locationActionGroup.enabled.state !== undefined
            Component.onCompleted:
                clicked.connect(locationOn.clicked)
        }

        Binding {
            target: locationOn
            property: "checked"
            value: locationActionGroup.enabled.state
        }

        ListItem.Caption {
            /* TODO: replace by real info from the location service */
            property int locationInfo: 0

            text: {
                if (locationInfo === 0) /* GPS only */
                    return i18n.tr("Uses GPS to detect your rough location. When off, GPS turns off to save battery.")
                else if (locationInfo === 1) /* GPS, wi-fi on */
                    return i18n.tr("Uses wi-fi and GPS to detect your rough location. Turning off location detection saves battery.")
                else if (locationInfo === 2) /* GPS, wi-fi off */
                    return i18n.tr("Uses wi-fi (currently off) and GPS to detect your rough location. Turning off location detection saves battery.")
                else if (locationInfo === 3) /* GPS, wi-fi and cellular on */
                    return i18n.tr("Uses wi-fi, cell tower locations, and GPS to detect your rough location. Turning off location detection saves battery.")
                else if (locationInfo === 4) /* GPS, wi-fi on, cellular off */
                    return i18n.tr("Uses wi-fi, cell tower locations (no current cellular connection), and GPS to detect your rough location. Turning off location detection saves battery.")
                else if (locationInfo === 5) /* GPS, wi-fi off, cellular on */
                    return i18n.tr("Uses wi-fi (currently off), cell tower locations, and GPS to detect your rough location. Turning off location detection saves battery.")
                else if (locationInfo === 6) /* GPS, wi-fi and cellular off */
                    return i18n.tr("Uses wi-fi (currently off), cell tower locations (no current cellular connection), and GPS to detect your rough location. Turning off location detection saves battery.")
            }

            visible: showAllUI
        }

        ListItem.Standard {
            text: i18n.tr("Allow access to location:")
            visible: showAllUI && locationOn.checked
        }

        Repeater {
            model: ["Browser", "Camera", "Clock", "Weather"]
            ListItem.Standard {
                text: modelData
                control: Switch { checked: true; enabled: false}
                visible: showAllUI && locationOn.checked
            }
        }
    }
}
