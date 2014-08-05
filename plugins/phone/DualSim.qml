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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/
import QtQuick 2.0
import GSettings 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import "../cellular/Components"

Column {

    anchors.left: parent.left
    anchors.right: parent.right

    Sim {
        id: sim1
        path: modemsSorted[0]
    }

    Sim {
        id: sim2
        path: modemsSorted[1]
    }

    ListItem.Standard {
        text: sim1.title
    }

    ListItem.Standard {
        text: i18n.tr("Call forwarding")
        progression: true
        onClicked: pageStack.push(Qt.resolvedUrl("CallForwarding.qml"), {
            modem: sim1.path,
            headerTitle: sim1.title
        })
    }

    ListItem.Standard {
        text: i18n.tr("Call waiting")
        progression: true
        onClicked: pageStack.push(Qt.resolvedUrl("CallWaiting.qml"), {
            modem: sim1.path,
            headerTitle: sim1.title
        })
    }

    ListItem.Standard {
        text: i18n.tr("Services")
        progression: true
        enabled: sim1.simMng.present
        onClicked: pageStack.push(Qt.resolvedUrl("Services.qml"), {
            carrierString: sim1.netReg.name,
            sim: sim1.simMng,
            headerTitle: sim1.title
        })
    }

    ListItem.Divider {}

    ListItem.Standard {
        text: sim2.title
    }

    ListItem.Standard {
        text: i18n.tr("Call forwarding")
        progression: true
        onClicked: pageStack.push(Qt.resolvedUrl("CallForwarding.qml"), {
            modem: sim2.path,
            headerTitle: sim2.title
        })
    }

    ListItem.Standard {
        text: i18n.tr("Call waiting")
        progression: true
        onClicked: pageStack.push(Qt.resolvedUrl("CallWaiting.qml"), {
            modem: sim2.path,
            headerTitle: sim2.title
        })
    }

    ListItem.Standard {
        text: i18n.tr("Services")
        progression: true
        enabled: sim2.simMng.present
        onClicked: pageStack.push(Qt.resolvedUrl("Services.qml"), {
            carrierString: sim2.netReg.name,
            sim: sim2.simMng,
            headerTitle: sim2.title
        })
    }


    GSettings {
        id: phoneSettings
        schema.id: "com.ubuntu.phone"
        Component.onCompleted: {
            // set default names
            var simNames = phoneSettings.simNames;
            var m0 = sim1.path
            var m1 = sim2.path
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
        target: sim1
        property: "name"
        value: phoneSettings.simNames[modemsSorted[0]]
    }

    Binding {
        target: sim2
        property: "name"
        value: phoneSettings.simNames[modemsSorted[1]]
    }

}
