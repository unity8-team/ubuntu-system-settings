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
import SystemSettings 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Column {

    property var sims

    Repeater {
        model: sims

        Column {

            anchors { left: parent.left; right: parent.right }

            SettingsItemTitle { text: sims[index].title }

            ListItem.Standard {
                objectName: "callWaitSim" + index
                text: i18n.tr("Call waiting")
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("CallWaiting.qml"), {
                    sim: sims[index],
                    headerTitle: sims[index].title
                })
            }

            ListItem.SingleValue {
                objectName: "callFwdSim" + index
                text: i18n.tr("Call forwarding")
                progression: true
                value: {
                    if (sims[index].callForwarding.voiceUnconditional) {
                        return i18n.tr("All calls");
                    } else if (sims[index].callForwarding.voiceBusy ||
                               sims[index].callForwarding.voiceNoReply ||
                               sims[index].callForwarding.voiceNotReachable) {
                        return i18n.tr("Some calls")
                    } else {
                        return i18n.tr("Off")
                    }
                }
                onClicked: pageStack.push(Qt.resolvedUrl("CallForwarding.qml"), {
                    sim: sims[index],
                    headerTitle: sims[index].title
                })
            }

            ListItem.Standard {
                objectName: "simServicesSim" + index
                text: i18n.tr("Services")
                progression: true
                enabled: sims[index].simMng.present
                showDivider: false
                onClicked: pageStack.push(Qt.resolvedUrl("Services.qml"), {
                    carrierString: sims[index].netReg.name,
                    sim: sims[index].simMng,
                    headerTitle: sims[index].title
                })
            }

            ListItem.Divider {}

            Binding {
                target: sims[index]
                property: "name"
                value: phoneSettings.simNames[modemsSorted[index]]
            }
        }
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
}
