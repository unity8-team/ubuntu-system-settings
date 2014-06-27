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
    objectName: "aboutPage"

    title: i18n.tr("About this phone")
    flickable: scrollWidget

    UbuntuStorageAboutPanel {
        id: backendInfos
    }

    DeviceInfo {
        id: deviceInfos
    }

    UpdateManager {
        id: updateBackend
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

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
                        objectName: "deviceLabel"
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: deviceInfos.manufacturer() ? deviceInfos.manufacturer() + " " + deviceInfos.model() : backendInfos.vendorString
                    }
                }
            }

            ListItem.SingleValue {
                id: serialItem
                objectName: "serialItem"
                text: i18n.tr("Serial")
                value: backendInfos.serialNumber
                visible: backendInfos.serialNumber
            }

            ListItem.SingleValue {
                objectName: "imeiItem"
                property string imeiNumber
                imeiNumber: deviceInfos.imei(0)
                text: "IMEI"
                value: imeiNumber
                visible: imeiNumber
            }

            ListItem.Divider {}

            ListItem.Standard {
                id: storageItem
                objectName: "storageItem"
                text: i18n.tr("Storage")
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("Storage.qml"))
            }

            ListItem.Standard {
                objectName: "softwareItem"
                text: i18n.tr("Software:")
            }

            ListItem.SingleValue {
                objectName: "osItem"
                text: i18n.tr("OS")
                value: "Ubuntu " + deviceInfos.version(DeviceInfo.Os) +
                       (updateBackend.currentBuildNumber ? " (r%1)".arg(updateBackend.currentBuildNumber) : "")
            }

            ListItem.SingleValue {
                objectName: "lastUpdatedItem"
                text: i18n.tr("Last updated")
                value: backendInfos.updateDate ? backendInfos.updateDate : i18n.tr("Never")
            }

            ListItem.SingleControl {
                control: Button {
                    objectName: "updateButton"
                    text: i18n.tr("Check for updates")
                    width: parent.width - units.gu(4)
                    onClicked:
                        pageStack.push(pluginManager.getByName("system-update").pageComponent)
                }
            }

            ListItem.Standard {
                objectName: "legalItem"
                text: i18n.tr("Legal:")
            }

            ListItem.Standard {
                objectName: "licenseItem"
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
