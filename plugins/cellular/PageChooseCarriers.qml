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

ItemPage {
    id: root
    title: i18n.tr("Carriers")
    objectName: "chooseCarriersPage"

    property var sim1
    property var sim2

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
                objectName: "chooseCarrierSim1"
                value: sim1.netReg.name ? sim1.netReg.name : i18n.tr("N/A")
                progression: true
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("PageChooseCarrier.qml"), {
                        netReg: sim1.netReg,
                        title: sim1.title
                    })
                }
            }

            ListItem.Standard {
                text: sim2.title
            }

            ListItem.SingleValue {
                objectName: "chooseCarrierSim2"
                value: sim2.netReg.name ? sim2.netReg.name : i18n.tr("N/A")
                progression: true
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("PageChooseCarrier.qml"), {
                        netReg: sim2.netReg,
                        title: sim2.title
                    })
                }
            }
        }
    }
}
