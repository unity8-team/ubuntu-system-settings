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
import GSettings 1.0
import MeeGo.QOfono 0.2
import Ubuntu.Components.ListItems 0.1 as ListItem

Column {

    property var paths

    ListItem.Standard {
        text: i18n.tr("Phone number:")
        visible: phoneNumber1.visible
    }

    ListItem.SingleValue {

        id: phoneNumber1
        objectName: "numberItem1"

        OfonoSimManager {
            id: sim1
            modemPath: paths[0]
        }
        property string phoneNumber
        phoneNumber: sim1.subscriberNumbers.length > 0 ?
            sim1.subscriberNumbers[0] : ""
        value: phoneNumber
        visible: value
    }

    ListItem.SingleValue {

        id: phoneNumber2
        objectName: "numberItem2"

        OfonoSimManager {
            id: sim2
            modemPath: paths[1]
        }
        property string phoneNumber
        phoneNumber: sim2.subscriberNumbers.length > 0 ?
            sim2.subscriberNumbers[0] : ""
        value: phoneNumber
        visible: value
    }

    GSettings {
        id: phoneSettings
        schema.id: "com.ubuntu.phone"
        Component.onCompleted: {
            // set default names
            var simNames = phoneSettings.simNames;
            var m0 = paths[0];
            var m1 = paths[1];
            if (!simNames[m0]) {
                simNames[m0] = "SIM 1";
            }
            if (!simNames[m1]) {
                simNames[m1] = "SIM 2";
            }
            phoneSettings.simNames = simNames;
        }
    }

    Binding {
        target: phoneNumber1
        property: "text"
        value: phoneSettings.simNames[paths[0]]
    }

    Binding {
        target: phoneNumber2
        property: "text"
        value: phoneSettings.simNames[paths[1]]
    }
}
