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
    title: i18n.tr("Carriers & APNs")
    objectName: "carrierApnPage"
    flickable: scrollWidget

    property var sims

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            Repeater {
                model: sims

                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    SettingsItemTitle {
                        text: sims[index].title
                    }

                    SettingsListItems.SingleValueProgression {
                        text: i18n.tr("Carrier")
                        objectName: sims[index].path + "_carriers"
                        value: sims[index].netReg.name ? sims[index].netReg.name :
                            i18n.tr("None")
                        enabled: (sims[index].netReg.status !== "") &&
                            (sims[index].netReg.mode !== "auto-only")
                        progressionVisible: enabled
                        onClicked: pageStack.addPageToNextColumn(root,
                            Qt.resolvedUrl("PageChooseCarrier.qml"), {
                            sim: sims[index],
                            title: sims[index].title
                        })
                    }

                    SettingsListItems.StandardProgression {
                        text: i18n.tr("APN")
                        progressionVisible: enabled
                        onClicked: pageStack.addPageToNextColumn(root,
                            Qt.resolvedUrl("PageChooseApn.qml"), {
                            sim: sims[index]
                        })
                    }
                }
            }
        }
    }
}
