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
import Ubuntu.Components 0.1
import SystemSettings 1.0
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.StorageAbout 1.0

ItemPage {
    id: root

    title: i18n.tr("About this phone")
    flickable: scrollWidget

    UbuntuStorageAboutPanel {
        id: backendInfos
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Base {
                // This should be treated like a ListItem.Standard, but with 2
                // rows.  So we'll set the height equal to that of one already
                // defined multipled by 2.
                height: storageItem.height * 2

                Item {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    height:  storageItem.height
                    Label {
                        id: ubuntuLabel
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            bottomMargin: units.gu(1)
                        }
                        text: ""
                        fontSize: "large"
                    }
                    Label {
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            topMargin: units.gu(1)
                            top: ubuntuLabel.bottom
                        }
                        text: (backendInfos.vendorString.toString() !== " ") ? backendInfos.vendorString : i18n.tr("Unknown Model")
                    }
                }
            }

            ListItem.SingleValue {
                id: serialItem
                text: i18n.tr("Serial")
                value: backendInfos.serialNumber ? backendInfos.serialNumber : i18n.tr("N/A")
            }

            ListItem.SingleValue {
                text: "IMEI"
                value: "FAKE-IMEI-ID-NUMBER"     // TODO: read IMEI number from the device
            }

            ListItem.Standard {
                text: i18n.tr("Software:")
            }

            ListItem.SingleValue {
                text: i18n.tr("OS")
                value: backendInfos.OsVersion
            }

            ListItem.SingleValue {
                text: i18n.tr("Last Updated")
                value: "2013-04-09"              // TODO: read update infos from the device
            }

            // TOFIX: use ListItem.SingleControl when lp #1194844 is fixed
            ListItem.Base {
                Button {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        left: parent.left
                    }
                    text: i18n.tr("Check for updates")
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
            }

            ListItem.Standard {
                text: i18n.tr("Regulatory info")
                progression: true
            }
        }
    }
}
