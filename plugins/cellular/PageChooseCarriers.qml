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
                objectName: sims[0].path + "_carriers"
                value: sims[0].netReg.name ? sims[0].netReg.name : i18n.tr("N/A")
                enabled: sims[0].netReg.status !== ""
                progression: true
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("PageChooseCarrier.qml"), {
                        sim: sims[0],
                        title: sims[0].title
                    })
                }
            }

            ListItem.Standard {
                text: sims[1].title
            }

            ListItem.SingleValue {
                objectName: sims[1].path + "_carriers"
                value: sims[1].netReg.name ? sims[1].netReg.name : i18n.tr("N/A")
                enabled: sims[1].netReg.status !== ""
                progression: true
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("PageChooseCarrier.qml"), {
                        sim: sims[1],
                        title: sims[1].title
                    })
                }
            }
        }
    }
}
