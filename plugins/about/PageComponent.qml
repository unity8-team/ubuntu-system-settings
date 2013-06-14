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

ItemPage {
    id: root

    title: i18n.tr("About this phone")
    flickable: scrollWidget

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: columnId.height

        Column {
            id: columnId

            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Base {
                height: ubuntuLogo.height + vendorItm.height + units.gu(1)
                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: ubuntuLogo.height
                        Item {
                            id: ubuntuLogo
                            height: childrenRect.height
                            width: childrenRect.width
                            Label {
                                id: ubuntuLogoName
                                text: "Ubuntu"
                                fontSize: "large"
                            }
                            Label {
                                anchors.left: ubuntuLogoName.right
                                text: ""
                                fontSize: "small"
                            }
                        }
                    }
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        Item {
                            id: vendorItm
                            height: vendorLabel.height + units.gu(1)
                            width: childrenRect.width
                            Label {
                                id: vendorLabel
                                text: "Vendor" + " " + "Model" // TODO: get manufactor and model infos from the system
                            }
                        }
                    }
                }
            }

            ListItem.SingleValue {
                id: serialItem
                text: i18n.tr("Serial")
                value: "FAKE-SERIAL-ID-NUMBER"   // TODO: read serial number from the device
            }

            ListItem.SingleValue {
                text: "IMEI"
                value: "FAKE-IMEI-ID-NUMBER"     // TODO: read IMEI number from the device
            }

            ListItem.Header {
                text: i18n.tr("Software:")
            }

            ListItem.SingleValue {
                text: i18n.tr("OS")
                value: "Ubuntu Version 0.3"      // TODO: read version number from the device
            }

            ListItem.SingleValue {
                text: i18n.tr("Last Updated")
                value: "2013-04-09"              // TODO: read update infos from the device
            }

            ListItem.SingleControl {
                control: Button {
                    anchors {
                        margins: units.gu(1)
                        fill: parent
                    }
                    text: i18n.tr("Check for updates")
                }
            }

            ListItem.Standard {
                text: i18n.tr("Storage")
                progression: true
            }

            ListItem.Header {
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
