/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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

import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.SystemSettings.Cellular 1.0
import Ubuntu.Components 1.3
import MeeGo.QOfono 0.2
import QMenuModel 0.1
import "Components" as LocalComponents
import "sims.js" as Sims

ItemPage {
    id: root
    title: i18n.tr("Cellular")
    flickable: flick
    objectName: "cellularPage"

    property var modemsSorted: []
    property int simsLoaded: 0

    // waiting during modem reboots or for modems to come online
    property bool waiting: true

    QtObject {
        id: priv
        property string prevOnlineModem: ""
    }

    states: [
        State {
            name: "waiting"
            when: waiting
            PropertyChanges {
                target: waitIndicator
                opacity: 1
            }
            PropertyChanges {
                target: flick
                opacity: 0
            }
        },
        State {
            name: "noSim"
            when: (simsLoaded === 0) || (Sims.getPresentCount() === 0)
            StateChangeScript {
                script: loader.source = "Components/NoSim.qml"
            }
        },
        State {
            name: "singleSim"
            StateChangeScript {
                script: loader.setSource("Components/SingleSim.qml", {
                    sim: Sims.getFirstPresent(),
                    prevOnlineModem: priv.prevOnlineModem
                })
            }
            when: simsLoaded && (Sims.getPresentCount() === 1)
        },
        State {
            name: "multiSim"
            StateChangeScript {
                script: loader.setSource("Components/MultiSim.qml", {
                    sims: Sims.getAll(),
                    modems: modemsSorted,
                    prevOnlineModem: priv.prevOnlineModem
                })
            }
            when: simsLoaded && (Sims.getPresentCount() > 1)
        }
    ]

    OfonoManager {
        id: manager
        onModemsChanged: {
            root.modemsSorted = modems.slice(0).sort();
            Sims.createQML();
            root.waiting = false;
        }
    }

    QDBusActionGroup {
        id: actionGroup
        busType: 1
        busName: "com.canonical.indicator.network"
        objectPath: "/com/canonical/indicator/network"

        property variant actionObject: action("wifi.enable")

        Component.onCompleted: {
            start()
        }
    }

    Item {
        id: waitIndicator
        anchors.fill: parent
        opacity: 0

        ActivityIndicator {
            anchors.centerIn: parent
            running: true
        }

        Behavior on opacity {
            PropertyAnimation {
               duration: UbuntuAnimation.SlowDuration
            }
        }
    }

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors { left: parent.left; right: parent.right }
            Loader {
                id: loader
                anchors { left: parent.left; right: parent.right }
            }
        }

        Behavior on opacity {
            PropertyAnimation {
                duration: UbuntuAnimation.SlowDuration
            }
        }

        Connections {
            target: loader.item

            onUmtsModemChanged: {
                var path = sim.path;
                var e = sim.simMng.presenceChanged;

                function presenceHandler (ispresent) {
                    if (ispresent) {
                        root.waiting = false;
                        Connectivity.unlockAllModems();
                        e.disconnect(presenceHandler);
                    }
                }

                priv.prevOnlineModem = prevOnlineModem ?
                    prevOnlineModem : "";
                root.waiting = true;

                /* When the SIM comes back online, set waiting to false:
                the modem reboot is done.*/
                sim.simMng.presenceChanged.connect(presenceHandler);
            }
            ignoreUnknownSignals: true
        }
    }
}
