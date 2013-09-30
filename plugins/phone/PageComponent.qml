/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
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

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Phone 1.0

ItemPage {
    title: i18n.tr("Phone")
    property string carrierName: netop.name
    property string carrierString: carrierName ? carrierName : i18n.tr("SIM")

    NetworkRegistration {
        id: netop;
        onNameChanged:
            carrierName = netop.name
    }

    SimManager {
        id: sim
    }

    Column {
        anchors.fill: parent

        ListItem.Standard {
            text: i18n.tr("Call forwarding")
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("CallForwarding.qml"))
        }

        ListItem.Standard {
            text: i18n.tr("Call waiting")
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("CallWaiting.qml"))
        }

        ListItem.Divider {}

        ListItem.Standard {
            // TRANSLATORS: %1 is the name of the (network) carrier
            text: i18n.tr("%1 Services").arg(carrierString)
            progression: true
            enabled: sim.present
            onClicked: pageStack.push(Qt.resolvedUrl("Services.qml"), {carrierString: carrierString, sim: sim})
        }
    }
}
