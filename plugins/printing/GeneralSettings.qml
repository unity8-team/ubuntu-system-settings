/*
 * Copyright 2017 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by Jonas G. Drange <jonas.drange@canonical.com>
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Settings.Components 0.1
import Ubuntu.Settings.Printers 0.1

ItemPage {
    id: generalSettingsPage
    objectName: "generalSettingsPage"
    title: i18n.tr("General settings")
    flickable: generalSettingsFlickable
    property var printer

    Flickable {
        id: generalSettingsFlickable
        anchors.fill: parent

        Column {
            spacing: units.gu(2)
            anchors {
                top: parent.top
                topMargin: units.gu(2)
                left: parent.left
                right: parent.right
            }

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: units.gu(2)
                }
                text: "Description"
            }

            ListItems.SingleControl {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                control: TextField {
                   anchors {
                        margins: units.gu(1)
                        left: parent.left
                        right: parent.right

                    }
                    text: printer.description
                    onTextChanged: printer.description = text
                }
            }


            ListItems.ValueSelector {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                enabled: values.length > 1
                text: "Duplex"
                values: printer.supportedDuplexModes
                onSelectedIndexChanged: printer.duplexMode = selectedIndex
                Component.onCompleted: {
                    if (enabled) {
                        selectedIndex = printer.duplexMode
                    }
                }
            }

            ListItems.ValueSelector {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: "Page size"
                values: printer.supportedPageSizes
                onSelectedIndexChanged: printer.pageSize = selectedIndex
                Component.onCompleted: selectedIndex = printer.supportedPageSizes.indexOf(printer.pageSize)
            }

            ListItems.ValueSelector {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                visible: printer.supportedColorModels.length
                text: "Color model"
                values: printer.supportedColorModels
                enabled: values.length > 1
                onSelectedIndexChanged: printer.colorModel = selectedIndex
                Component.onCompleted: {
                    if (enabled)
                        selectedIndex = printer.colorModel
                }
            }

            ListItems.ValueSelector {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                visible: printer.supportedPrintQualities.length
                text: "Quality"
                values: printer.supportedPrintQualities
                enabled: values.length > 1
                onSelectedIndexChanged: printer.printQuality = selectedIndex
                Component.onCompleted: {
                    if (enabled)
                        selectedIndex = printer.printQuality
                }
            }
        }
    }
}
