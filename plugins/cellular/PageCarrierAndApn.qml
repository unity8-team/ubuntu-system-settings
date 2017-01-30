/*
 * Copyright (C) 2014-2016 Canonical Ltd
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
import QtQuick 2.4
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3

ItemPage {
    id: root
    title: i18n.tr("Carrier & APN")
    objectName: "carrierApnPage"
    flickable: scrollWidget

    property var sim

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            SettingsListItems.SingleValueProgression {
                text: i18n.tr("Carrier")
                objectName: "carrier"
                value: sim.netReg.name ? sim.netReg.name : i18n.tr("None")
                enabled: (sim.netReg.status !== "") &&
                    (sim.netReg.mode !== "auto-only")
                progressionVisible: enabled
                onClicked: pageStack.addPageToNextColumn(root,
                    Qt.resolvedUrl("PageChooseCarrier.qml"), {
                    sim: sim,
                    title: i18n.tr("Carrier")
                })
            }

            SettingsListItems.SingleValueProgression {
                text: i18n.tr("APN")
                objectName: "apn"
                progressionVisible: enabled
                onClicked: pageStack.addPageToNextColumn(root,
                    Qt.resolvedUrl("PageChooseApn.qml"), {
                    sim: sim
                })
            }
        }
    }
}
