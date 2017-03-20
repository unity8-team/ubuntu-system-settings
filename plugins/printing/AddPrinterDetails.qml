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

import QtQml 2.2
import QtQuick 2.4
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.Extras.Printers 0.1
import Ubuntu.Components.ListItems 1.3 as ListItems

ItemPage {
    id: describePrinterPage
    objectName: "printingPage"
    property var device: null
    signal printerAdded()

    header: PageHeader {
        id: printerHeader
        title: i18n.tr("Describe printer")
        flickable: addPrinterFlickable
        leadingActionBar {
            actions: [
                Action {
                    id: closeAction
                    iconName: "back"
                    text: i18n.tr("Back")
                    onTriggered: pageStack.removePages(describePrinterPage)
                }
            ]
        }
        trailingActionBar {
            actions: [
                Action {
                    id: addAction
                    iconName: "ok"
                    text: i18n.tr("Add printer")
                    enabled: connectionsSelector.selectedIndex > 0
                    onTriggered: {
                        describePrinterPage.state = "adding";
                        var ret;
                        if (driverSelector.selectedIndex == 0) {
                            ret = Printers.addPrinter(
                                nameField.text,
                                driversView.selectedDriver,
                                connection.host,
                                descriptionField.text,
                                locationField.text
                            );
                        } else {
                            ret = Printers.addPrinterWithPpdFile(
                                nameField.text,
                                pddFileField.text,
                                connection.host,
                                descriptionField.text,
                                locationField.text
                            );
                        }
                        if (ret) {
                            describePrinterPage.state = "adding"
                        } else {
                            errorMessage.text = Printers.lastMessage;
                            describePrinterPage.state = "failure"
                        }
                    }
                }
            ]
        }
    }

    Component.onCompleted: {
        var suggestedName;

        if (device) {
            driverFilter.text = device.displayName;

            suggestedName = (" " + device.displayName).slice(1);
            suggestedName = suggestedName.replace(/\ /g, "\-");
            nameField.text = suggestedName;

            connectionsSelector.selectedIndex = connectionsSelector.values.indexOf(connection.typeToString(device.type));
            connection.host = device.uri;

            descriptionField.text = device.info;
            locationField.text = device.location;
        }
    }

    states: [
        State {
            name: "adding"
            PropertyChanges { target: closeAction; enabled: false }
            PropertyChanges { target: addAction; enabled: false }
            PropertyChanges { target: successTimer; running: true }
            PropertyChanges { target: connection; enabled: false }
            PropertyChanges { target: fieldsColumn; enabled: false }
        },
        State {
            name: "failure"
            PropertyChanges { target: errorMessageContainer; visible: true }
        }
    ]

    Flickable {
        id: addPrinterFlickable
        anchors.fill: parent

        Column {
            id: fieldsColumn
            anchors { left: parent.left; right: parent.right }
            visible: connectionsSelector.selectedIndex > 0
            height: visible ? childrenRect.height : 0
            clip: true
            property bool enabled: true

            ListItems.ValueSelector {
                id: connectionsSelector
                anchors { left: parent.left; right: parent.right }
                text: i18n.tr("Connection")
                values: [
                    i18n.tr("Choose a connection"),
                    "ipp",
                    "lpd",
                    "ipps",
                    "ipp14",
                    "http",
                    "beh",
                    "socket",
                    "https",
                    "ipp",
                    "hp",
                    "usb",
                    "hpfax",
                    "dnssd",
                ]
                onSelectedIndexChanged: {
                    if (selectedIndex === 0) {
                        connection.type = PrinterEnum.IppType;
                    } else {
                        connection.type = connection.stringToType(values[selectedIndex]);
                    }
                }
            }

            SettingsListItems.Standard {
                text: i18n.tr("Printer name")
                anchors {
                    left: parent.left
                    right: parent.right
                }

                TextField {
                    id: nameField
                    enabled: fieldsColumn.enabled
                }
            }

            Item {
                id: errorMessageContainer
                visible: false
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: units.gu(2)
                }
                height: units.gu(6)
                Label {
                    id: errorMessage
                    anchors {
                        top: parent.top
                        topMargin: units.gu(2)
                        horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            PrinterConnection {
                id: connection
                anchors { left: parent.left; right: parent.right }
                onTypeChanged: {
                    var selIndex = connectionsSelector.values.indexOf(typeToString(type));
                    if (selIndex < 0) {
                        selIndex = 0;
                    }
                    connectionsSelector.selectedIndex = selIndex;
                }
            }

            SettingsListItems.Standard {
                text: i18n.tr("Description")
                anchors { left: parent.left; right: parent.right }

                TextField {
                    id: descriptionField
                    placeholderText: i18n.tr("Optional")
                    enabled: fieldsColumn.enabled
                }
            }

            SettingsListItems.Standard {
                text: i18n.tr("Location")
                anchors { left: parent.left; right: parent.right }

                TextField {
                    id: locationField
                    placeholderText: i18n.tr("Optional")
                    enabled: fieldsColumn.enabled && showAllUI
                }
            }

            SettingsItemTitle {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: i18n.tr("Driver")
            }

            ListItems.ValueSelector {
                id: driverSelector
                anchors { left: parent.left; right: parent.right }
                text: i18n.tr("Choose driver")
                values: [
                    i18n.tr("Select printer from database"),
                    i18n.tr("Provide PPD file")
                ]
                enabled: parent.enabled
            }

            SettingsListItems.Standard {
                anchors { left: parent.left; right: parent.right }
                text: i18n.tr("Filter drivers")

                TextField {
                    id: driverFilter
                    onTextChanged: Printers.driverFilter = text
                    enabled: fieldsColumn.enabled
                }
                visible: driverSelector.selectedIndex == 0
                enabled: parent.enabled
            }

            ScrollView {
                anchors { left: parent.left; right: parent.right }
                height: units.gu(30)
                contentItem: driversView
                visible: driverSelector.selectedIndex == 0

                ListView {
                    id: driversView
                    property string selectedDriver
                    property bool loading: true
                    model: Printers.drivers
                    anchors { left: parent.left; right: parent.right }
                    height: units.gu(30)
                    clip: true
                    enabled: fieldsColumn.enabled
                    currentIndex: -1
                    highlightFollowsCurrentItem: false
                    highlight: Rectangle {
                        z: 0
                        y: driversView.currentItem.y
                        width: driversView.currentItem.width
                        height: driversView.currentItem.height
                        color: theme.palette.selected.background
                    }
                    delegate: ListItem {
                        id: driverItem
                        height: driverLayout.height + (divider.visible ? divider.height : 0)

                        ListItemLayout {
                            id: driverLayout
                            title.text: displayName

                            Icon {
                                name: "ok"
                                width: units.gu(2)
                                height: width
                                SlotsLayout.position: SlotsLayout.Trailing
                                visible: model.name == driversView.selectedDriver
                            }
                        }
                        onClicked: {
                            driversView.selectedDriver = name
                            driversView.currentIndex = index
                        }
                    }

                    ActivityIndicator {
                        anchors.centerIn: parent
                        running: parent.loading
                    }

                    Connections {
                        target: driversView
                        onCountChanged: {
                            target = null;
                            driversView.loading = false;
                        }
                    }
                }
            }

            SettingsListItems.Standard {
                id: pddFileField
                anchors { left: parent.left; right: parent.right }
                enabled: parent.enabled
                text: i18n.tr("PPD file")
                visible: driverSelector.selectedIndex == 1

                TextField {
                    id: printerPpd
                    enabled: parent.enabled
                }
            }
        }
    }

    Timer {
        id: successTimer
        interval: 2000
        onTriggered: describePrinterPage.printerAdded()
    }
}
