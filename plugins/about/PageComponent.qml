/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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

import QtQuick 2.0
import QtSystemInfo 5.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.StorageAbout 1.0
import Ubuntu.SystemSettings.Update 1.0

ItemPage {
    id: root

    title: i18n.tr("About this phone")
    flickable: scrollWidget

    UbuntuStorageAboutPanel {
        id: backendInfos
    }

    DeviceInfo {
        id: deviceInfos
    }

    UbuntuUpdatePanel {
        id: updateBackend
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Base {
                height: ubuntuLabel.height + deviceLabel.height + units.gu(6)

                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.centerIn: parent
                    spacing: units.gu(2)
                    Label {
                        id: ubuntuLabel
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: ""
                        fontSize: "x-large"
                    }
                    Label {
                        id: deviceLabel
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: deviceInfos.manufacturer() ? deviceInfos.manufacturer() + " " + deviceInfos.model() : backendInfos.vendorString
                    }
                }
            }

            ListItem.SingleValue {
                id: serialItem
                text: i18n.tr("Serial")
                value: backendInfos.serialNumber ? backendInfos.serialNumber : i18n.tr("N/A")
            }

            ListItem.SingleValue {
                property string imeiNumber
                imeiNumber: deviceInfos.imei(0)
                text: "IMEI"
                value: imeiNumber ? imeiNumber : i18n.tr("N/A")
            }

            ListItem.Standard {
                text: i18n.tr("Software:")
            }

            ListItem.SingleValue {
                text: i18n.tr("OS")
                value: "Ubuntu " + deviceInfos.version(DeviceInfo.Os) +
                       (updateBackend.currentBuildNumber ? " (r%1)".arg(updateBackend.currentBuildNumber) : "")
            }

            ListItem.SingleValue {
                text: i18n.tr("Last updated")
                value: backendInfos.updateDate ? backendInfos.updateDate : i18n.tr("Never")
            }

            ListItem.SingleControl {
                control: Button {
                    text: i18n.tr("Check for updates")
                    width: parent.width - units.gu(4)
                    onClicked:
                        pageStack.push(pluginManager.getByName("system-update").pageComponent)
                }
            }

            ListItem.Standard {
                id: storageItem
                text: i18n.tr("Storage")
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("Storage.qml"))
            }

            ListItem.Standard {
                text: i18n.tr("Legal:")
            }

            ListItem.Standard {
                text: i18n.tr("Software licenses")
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("Software.qml"))
            }

            ListItem.Standard {
                property var regulatoryInfo:
                    pluginManager.getByName("regulatory-information")
                text: i18n.tr("Regulatory info")
                progression: true
                visible: regulatoryInfo
                onClicked: pageStack.push(regulatoryInfo.pageComponent)
            }
        }
    }
}
