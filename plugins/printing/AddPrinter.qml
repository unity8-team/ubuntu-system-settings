/*
 * This file is part of system-settings
 *
 * Copyright (C) 2017 Canonical Ltd.
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
 * Authored by Jonas G. Drange <jonas.drange@canonical.com>
 */

import QtQuick 2.4
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.Extras.Printers 0.1
import Ubuntu.Components.ListItems 1.3 as ListItems

ItemPage {
    id: addPrinterPage
    objectName: "printingPage"

    header: PageHeader {
        id: printerHeader
        title: i18n.tr("Add printer")
        flickable: addPrinterFlickable
        leadingActionBar {
            actions: [
                Action {
                    id: closeAction
                    iconName: "close"
                    text: i18n.tr("Cancel")
                    onTriggered: pageStack.removePages(addPrinterPage)
                }
            ]
        }
    }

    Component.onCompleted: Printers.prepareToAddPrinter()

    Flickable {
        id: addPrinterFlickable
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height + units.gu(4) // margin

        Column {
            anchors { left: parent.left; right: parent.right }
            height: childrenRect.height

            ListItem {
                height: layout.height + (divider.visible ? divider.height : 0)
                anchors { left: parent.left; right: parent.right }
                ListItemLayout {
                    id: layout
                    title.text: i18n.tr("Enter URI")
                    ProgressionSlot {}
                }
                onClicked: detailsPageObserver.target = pageStack.addFileToNextColumnSync(
                    addPrinterPage, Qt.resolvedUrl("AddPrinterDetails.qml"), {
                        plugin: plugin
                    }
                )
            }

            SettingsItemTitle {
                id: remotePrintersTitle
                anchors { left: parent.left; right: parent.right }
                text: i18n.tr("Devices")

                ActivityIndicator {
                    id: remotePrintersSearchIndicator
                    anchors {
                        right: parent.right
                        rightMargin: units.gu(2)
                        top: parent.top
                        topMargin: units.gu(2)

                    }
                    property var target
                    Component.onCompleted: target = Printers.devices
                    running: target.searching
                }
            }

            ListItems.ThinDivider {
                visible: remotePrintersList.count
            }

            Repeater {
                id: remotePrintersList
                anchors { left: parent.left; right: parent.right }
                model: Printers.devices
                delegate: ListItem {
                    height: modelLayout.height + (divider.visible ? divider.height : 0)
                    anchors { left: parent.left; right: parent.right }
                    ListItemLayout {
                        id: modelLayout
                        title.text: displayName ? displayName : info
                        subtitle.text: info

                        Icon {
                            id: icon
                            width: height
                            height: units.gu(2.5)
                            name: "network-printer-symbolic"
                            SlotsLayout.position: SlotsLayout.First
                        }

                        ProgressionSlot {}
                    }
                    onClicked: {
                        detailsPageObserver.target = pageStack.addFileToNextColumnSync(
                            addPrinterPage, Qt.resolvedUrl("AddPrinterDetails.qml"), {
                                device: model, plugin: plugin
                            }
                        )
                    }
                }
            }

            Label {
                anchors { left: parent.left; right: parent.right; margins: units.gu(2) }
                text: i18n.tr("No devices found.")
                visible: !remotePrintersSearchIndicator.running && remotePrintersList.count == 0
            }
        }
    }

    Connections {
        id: detailsPageObserver
        ignoreUnknownSignals: true
        onPrinterAdded: pageStack.removePages(addPrinterPage)
    }
}
