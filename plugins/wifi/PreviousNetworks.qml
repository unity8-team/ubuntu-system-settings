/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Wifi 1.0
import QMenuModel 0.1

ItemPage {
    id: previousNetworks
    objectName: "previousNetworksPage"
    title: i18n.tr("Previous networks")

    PreviousNetworkModel {
        id: pnmodel
    }

    ListView {
        id: networkList
        anchors.fill : parent
        model: pnmodel
        remove: Transition {
            ParallelAnimation {
                NumberAnimation {
                    property: "opacity"
                    to: 0
                    duration: UbuntuAnimation.SnapDuration
                }
                NumberAnimation {
                    property: "height"
                    to: 0
                    duration: UbuntuAnimation.SnapDuration
                }
            }
        }
        removeDisplaced: Transition {
            NumberAnimation {
                property: "y"
                duration: UbuntuAnimation.SnapDuration
            }
        }
        delegate: ListItem.Standard {
            text: name
            progression: true
            onClicked: {
                pageStack.push(Qt.resolvedUrl("NetworkDetails.qml"),
                {networkName : name, password : password, lastUsed : lastUsed,
                dbusPath : objectPath});
            }
        }
    }
}
