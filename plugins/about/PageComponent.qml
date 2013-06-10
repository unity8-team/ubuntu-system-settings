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

    title: i18n.tr("About This Phone")
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
                height: ubuntuLogo.height + vendorStr.height + 5
                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    Image {
                        id: ubuntuLogo
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: "/lib/plymouth/ubuntu_logo.png" // TODO: find better logo
                    }
                    Label {
                        id: vendorStr
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: i18n.tr("Vendor") + " " + i18n.tr("Model") // TODO: get manufactor and model infos from the system
                    }
                }
            }

            ListItem.SingleValue {
                text: i18n.tr("Serial")
                value: "FAKE-SERIAL-ID-NUMBER"   // TODO: read serial number from the device
            }

            ListItem.SingleValue {
                text: i18n.tr("IMEI")
                value: "FAKE-IMEI-ID-NUMBER"     // TODO: read IMEI number from the device
            }

            ListItem.Standard {
                text: i18n.tr("Software:")
            }

            ListItem.SingleValue {
                text: i18n.tr("OS:")
                value: "Ubuntu Version 0.3"      // TODO: read version number from the device
            }

            ListItem.SingleValue {
                text: i18n.tr("Last Updated")
                value: "2013-04-09"              // TODO: read update infos from the device
            }

            ListItem.Base {
                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    Button {
                        anchors {
                            margins: units.gu(3)
                            left: parent.left
                            right: parent.right
                        }
                        text: i18n.tr("Check for Updates")
                    }
                    Label {
                        text:i18n.tr("Legal:")
                    }
                }
            }

            ListItem.Standard {
                text: i18n.tr("Software Licenses")
                progression: true
            }

            ListItem.Standard {
                text: i18n.tr("Regulatory Info")
                progression: true
            }
        }
    }
}
