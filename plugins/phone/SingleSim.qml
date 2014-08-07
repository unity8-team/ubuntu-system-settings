/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Ken Vandine <ken.vandine@canonical.com>
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/
import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import "../cellular/Components"

Column {

    anchors.left: parent.left
    anchors.right: parent.right

    property string carrierName: sim.netReg.name
    property string carrierString: carrierName ? carrierName : i18n.tr("SIM")

    Sim {
        id: sim
        path: modemsSorted[0]
    }

    ListItem.Standard {
        objectName: "callFwd"
        text: i18n.tr("Call forwarding")
        progression: true
        onClicked: pageStack.push(Qt.resolvedUrl("CallForwarding.qml"), {modem: sim.path})
    }

    ListItem.Standard {
        objectName: "callWait"
        text: i18n.tr("Call waiting")
        progression: true
        onClicked: pageStack.push(Qt.resolvedUrl("CallWaiting.qml"), {modem: sim.path})
        showDivider: false
    }

    ListItem.Divider {}

    ListItem.Standard {
        objectName: "simServices"
        // TRANSLATORS: %1 is the name of the (network) carrier
        text: i18n.tr("%1 Services").arg(carrierString)
        progression: true
        enabled: sim.simMng.present
        onClicked: pageStack.push(Qt.resolvedUrl("Services.qml"),
                                  {carrierString: carrierString, sim: sim.simMng})
    }
}
