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
import GSettings 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem

Column {

    property var sims

    ListItem.Standard {
        text: sims[0].title
    }

    ListItem.Standard {
        objectName: "callFwdSim1"
        text: i18n.tr("Call forwarding")
        progression: true
        onClicked: pageStack.push(Qt.resolvedUrl("CallForwarding.qml"), {
            sim: sims[0],
            headerTitle: sims[0].title
        })
    }

    ListItem.Standard {
        objectName: "callWaitSim1"
        text: i18n.tr("Call waiting")
        progression: true
        onClicked: pageStack.push(Qt.resolvedUrl("CallWaiting.qml"), {
            sim: sims[0],
            headerTitle: sims[0].title
        })
    }

    ListItem.Standard {
        objectName: "simServicesSim1"
        text: i18n.tr("Services")
        progression: true
        enabled: sims[0].simMng.present
        onClicked: pageStack.push(Qt.resolvedUrl("Services.qml"), {
            carrierString: sims[0].netReg.name,
            sim: sims[0].simMng,
            headerTitle: sims[0].title
        })
    }

    ListItem.Divider {}

    ListItem.Standard {
        text: sims[1].title
    }

    ListItem.Standard {
        objectName: "callFwdSim2"
        text: i18n.tr("Call forwarding")
        progression: true
        onClicked: pageStack.push(Qt.resolvedUrl("CallForwarding.qml"), {
            sim: sims[1],
            headerTitle: sims[1].title
        })
    }

    ListItem.Standard {
        objectName: "callWaitSim2"
        text: i18n.tr("Call waiting")
        progression: true
        onClicked: pageStack.push(Qt.resolvedUrl("CallWaiting.qml"), {
            sim: sims[1],
            headerTitle: sims[1].title
        })
    }

    ListItem.Standard {
        objectName: "simServicesSim2"
        text: i18n.tr("Services")
        progression: true
        enabled: sims[1].simMng.present
        onClicked: pageStack.push(Qt.resolvedUrl("Services.qml"), {
            carrierString: sims[1].netReg.name,
            sim: sims[1].simMng,
            headerTitle: sims[1].title
        })
    }


    GSettings {
        id: phoneSettings
        schema.id: "com.ubuntu.phone"
        Component.onCompleted: {
            // set default names
            var simNames = phoneSettings.simNames;
            var m0 = sims[0].path
            var m1 = sims[1].path
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
        target: sims[0]
        property: "name"
        value: phoneSettings.simNames[modemsSorted[0]]
    }

    Binding {
        target: sims[1]
        property: "name"
        value: phoneSettings.simNames[modemsSorted[1]]
    }

}
