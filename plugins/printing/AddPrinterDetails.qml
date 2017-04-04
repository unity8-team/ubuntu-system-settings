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
    property var connections: null
    property var device: null
    signal printerAdded()

    function updateSettingsFromDevice(device) {
        var suggestedName;
        driverFilter.field.text = device.makeModel;

        suggestedName = (" " + device.info).slice(1);
        suggestedName = suggestedName.replace(/\ /g, "\-");
        nameField.field.text = suggestedName;
        hostField.field.text = device.uri;
        descriptionField.field.text = device.info;
        locationField.field.text = device.location;
    }

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
                    onTriggered: {
                        describePrinterPage.state = "adding";
                        var ret;
                        if (driverSelector.selectedIndex == 0) {
                            ret = Printers.addPrinter(
                                nameField.field.text,
                                driversView.selectedDriver,
                                hostField.field.text,
                                descriptionField.field.text,
                                locationField.field.text
                            );
                        } else {
                            ret = Printers.addPrinterWithPpdFile(
                                nameField.field.text,
                                pddFileField.field.text,
                                hostField.field.text,
                                descriptionField.field.text,
                                locationField.field.text
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
        if (device) {
            updateSettingsFromDevice(device);
        }
    }

    Component.onDestruction: Printers.driverFilter = ""

    states: [
        State {
            name: "adding"
            PropertyChanges { target: closeAction; enabled: false }
            PropertyChanges { target: addAction; enabled: false }
            PropertyChanges { target: successTimer; running: true }
            PropertyChanges { target: hostField; enabled: false }
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
        contentHeight: contentItem.childrenRect.height

        Column {
            id: fieldsColumn
            anchors { left: parent.left; right: parent.right }

            clip: true
            property bool enabled: true


            SettingsItemTitle {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: i18n.tr("Connection")
                visible: connections && connections.devices.count > 0
            }

            OptionSelector {
                id: connectionsSelector
                anchors {
                    margins: units.gu(2)
                    left: parent.left
                    right: parent.right
                }
                model: connections ? connections.devices : null
                visible: connections && connections.devices.count > 0
                delegate: OptionSelectorDelegate {
                    property var device: model
                    anchors { left: parent.left; right: parent.right }
                    text: displayName
                    subText: {
                        var protocol = i18n.tr("Unknown");
                        switch (Number(model.type)) {
                        case PrinterEnum.LPDType:
                            protocol = "LPD";
                            break;
                        case PrinterEnum.IppType:
                        case PrinterEnum.IppSType:
                        case PrinterEnum.Ipp14Type:
                            protocol = "IPP";
                            break;
                        case PrinterEnum.BehType:
                            protocol = "BEH";
                            break;
                        case PrinterEnum.SocketType:
                            protocol = i18n.tr("AppSocket/HP JetDirect");
                            break;
                        case PrinterEnum.HttpType:
                        case PrinterEnum.HttpsType:
                            protocol = "HTTP";
                            break;
                        case PrinterEnum.HPType:
                            protocol = i18n.tr("HP Linux Imaging and Printing (HPLIP)");
                            break;
                        case PrinterEnum.USBType:
                            protocol = "USB";
                            break;
                        case PrinterEnum.HPFaxType:
                            protocol = i18n.tr("Fax — HP Linux Imaging and Printing (HPLIP)");
                            break;
                        case PrinterEnum.DNSSDType:
                            protocol = "DNS-SD";
                            break;
                        }

                        /* TRANSLATORS: %1 is the way we identify the printer,
                        and %2 is the protocol via which we can connect to it.
                        So e.g. “Mark's HP LaserJet 5000 via USB”. */
                        return i18n.tr("%1 via %2").arg(info || makeModel).arg(protocol);
                    }
                }
                onDelegateClicked: updateSettingsFromDevice(
                    connections.devices.get(index)
                )
            }

            TextBoxListItem {
                id: nameField
                text: i18n.tr("Printer name")
                enabled: fieldsColumn.enabled
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

            TextBoxListItem {
                id: hostField
                text: i18n.tr("Host")
                enabled: fieldsColumn.enabled
            }

            TextBoxListItem {
                id: descriptionField
                text: i18n.tr("Description")
                enabled: fieldsColumn.enabled
                placeholderText: i18n.tr("Optional")
            }

            TextBoxListItem {
                id: locationField
                text: i18n.tr("Location")
                enabled: fieldsColumn.enabled
                placeholderText: i18n.tr("Optional")
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

            TextBoxListItem {
                id: driverFilter
                text: i18n.tr("Filter drivers")
                enabled: fieldsColumn.enabled
                visible: driverSelector.selectedIndex == 0
                onFieldTextChanged: Printers.driverFilter = fieldText
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

            TextBoxListItem {
                id: pddFileField
                text: i18n.tr("PPD file")
                enabled: parent.enabled
                visible: driverSelector.selectedIndex == 1
            }
        }
    }

    Timer {
        id: successTimer
        interval: 2000
        onTriggered: describePrinterPage.printerAdded()
    }
}
