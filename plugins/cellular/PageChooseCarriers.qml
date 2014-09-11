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
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import MeeGo.QOfono 0.2

ItemPage {
    id: root
    title: i18n.tr("Carriers")
    objectName: "chooseCarriersPage"

    property var sims

    Flickable {
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Standard {
                text: sims[0].title
            }

            ListItem.SingleValue {
                objectName: "chooseCarrierSim1"
                value: sims[0].netReg.name ? sims[0].netReg.name : i18n.tr("N/A")
                progression: true
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("PageChooseCarrier.qml"), {
                        netReg: sims[0].netReg,
                        title: sims[0].title
                    })
                }
            }

            ListItem.Standard {
                text: sims[1].title
            }

            ListItem.SingleValue {
                objectName: "chooseCarrierSim2"
                value: sims[1].netReg.name ? sims[1].netReg.name : i18n.tr("N/A")
                progression: true
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("PageChooseCarrier.qml"), {
                        netReg: sims[1].netReg,
                        title: sims[1].title
                    })
                }
            }
        }
    }
}
