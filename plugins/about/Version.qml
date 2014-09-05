/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
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
 *
 * Author: Sergio Schvezov <sergio.schvezov@canonical.com>
 *
 */

import QtQuick 2.0
import Qt.labs.folderlistmodel 1.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.StorageAbout 1.0
import Ubuntu.SystemSettings.Update 1.0

ItemPage {
    id: versionPage
    objectName: "versionPage"
    title: i18n.tr("OS Build Details")

    UbuntuStorageAboutPanel {
        id: storedInfo
    }

    Flickable {
        anchors.fill: parent

        Column {
            anchors.fill: parent

            ListItem.SingleValue {
                objectName: "versionBuildNumberItem"
                text: i18n.tr("OS build number")
                value: UpdateManager.currentBuildNumber ? UpdateManager.currentBuildNumber : "Non system image"
            }

            ListItem.SingleValue {
                objectName: "ubuntuVersionBuildNumberItem"
                text: i18n.tr("Ubuntu Image part")
                value: UpdateManager.currentUbuntuBuildNumber
                visible: UpdateManager.currentUbuntuBuildNumber
            }

            ListItem.SingleValue {
                objectName: "ubuntuBuildIDItem"
                height: ubuntuBuildIDColumn.childrenRect.height + units.gu(2)
                visible: storedInfo.ubuntuBuildID
                anchors {
                    left: parent.left
                    right: parent.right
                }

                Column {
                    anchors.fill: parent
                    anchors.topMargin: units.gu(1)

                    id: ubuntuBuildIDColumn
                    spacing: units.gu(1)
                    Label {
                        anchors {
                            left:parent.left
                            right:parent.right
                        }
                        wrapMode: Text.WordWrap
                        text: i18n.tr("Ubuntu build description")
                    }
                    Label {
                        anchors {
                            left:parent.left
                            right:parent.right
                        }
                        wrapMode: Text.WordWrap
                        text: storedInfo.ubuntuBuildID
                    }
                }
            }

            ListItem.SingleValue {
                objectName: "deviceVersionBuildNumberItem"
                text: i18n.tr("Device Image part")
                value: UpdateManager.currentDeviceBuildNumber
                visible: UpdateManager.currentDeviceBuildNumber
            }

            ListItem.SingleValue {
                objectName: "deviceBuildIDItem"
                height: deviceBuildIDColumn.childrenRect.height + units.gu(2)
                visible: storedInfo.deviceBuildDisplayID
                anchors {
                    left: parent.left
                    right: parent.right
                }

                Column {
                    anchors.fill: parent
                    anchors.topMargin: units.gu(1)

                    id: deviceBuildIDColumn
                    spacing: units.gu(1)
                    Label {
                        anchors {
                            left:parent.left
                            right:parent.right
                        }
                        wrapMode: Text.WordWrap
                        text: i18n.tr("Device build description")
                    }
                    Label {
                        anchors {
                            left:parent.left
                            right:parent.right
                        }
                        wrapMode: Text.WordWrap
                        text: storedInfo.deviceBuildDisplayID
                    }
                }
            }

            ListItem.SingleValue {
                objectName: "customizationBuildNumberItem"
                text: i18n.tr("Customization Image part")
                value: storedInfo.customizationBuildID
                visible: storedInfo.customizationBuildID
            }
        }
    }
}

