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
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import MeeGo.QOfono 0.2

ItemPage {
    id: root
    title: i18n.tr("Carriers")
    objectName: "carriersPage"

    property var netReg
    property var sim1
    property var sim2

    OfonoNetworkRegistration {
        id: netReg2
        modemPath: sim2.path
        onStatusChanged: {
            console.warn ("netReg2 onStatusChanged: " + status, modemPath);
        }
        onModeChanged: {
            console.warn ("netReg2 onModeChanged: " + mode, modemPath);
        }
    }

    Flickable {
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Standard {
                text: sim1.title
            }

            ListItem.SingleValue {
                value: netReg.name ? netReg.name : i18n.tr("N/A")
                progression: true
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("ChooseCarrier.qml"), {
                        netReg: netReg,
                        title: sim1.title
                    })
                }
            }

            ListItem.Standard {
                text: sim2.title
            }

            ListItem.SingleValue {
                value: netReg2.name ? netReg2.name : i18n.tr("N/A")
                progression: true
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("ChooseCarrier.qml"), {
                        netReg: netReg2,
                        title: sim2.title
                    })
                }
            }
        }
    }
}
