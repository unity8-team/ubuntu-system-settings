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
import QtSystemInfo 5.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

ItemPage {
    title: i18n.tr("Phone")
    property string carrierName: infoBackend.networkName(NetworkInfo.GsmMode, 0)
    property string carrierString: carrierName ? carrierName : i18n.tr("SIM")

    NetworkInfo {
        id: infoBackend;
        monitorNetworkName: true
        onNetworkNameChanged:
            carrierName = infoBackend.networkName(NetworkInfo.GsmMode, 0)
    }

    Column {
        anchors.fill: parent

        ListItem.Standard {
            text: i18n.tr("Call diversion")
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("CallDiversion.qml"))
        }

        ListItem.Standard {
            text: i18n.tr("Call waiting")
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("CallWaiting.qml"))
        }

        ListItem.Divider {}

        ListItem.Standard {
            text: i18n.tr("%1 Services").arg(carrierString)
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("Services.qml"), {carrierString: carrierString})
        }
    }
}
