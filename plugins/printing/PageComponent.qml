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
import Ubuntu.Components 1.3
import Ubuntu.Components.Extras.Printers 0.1

ItemPage {
    id: root
    objectName: "printingPage"

    header: PageHeader {
        id: printerHeader
        title: i18n.tr("Printing")
        flickable: printerList
        trailingActionBar {
            actions: [
                Action {
                    iconName: "add"
                    text: i18n.tr("Add printer")
                    onTriggered: pageStack.addPageToNextColumn(root, Qt.resolvedUrl("AddPrinter.qml"), {
                        plugin: plugin
                    })
                },
                Action {
                    iconName: "document-print"
                    text: i18n.tr("Manager Printer Jobs")
                    onTriggered: Qt.openUrlExternally("printing:///queue")
                }
            ]
        }
    }

    onPushedOntoStack: {
        var page;
        var opts = {
            plugin: plugin,
            pluginManager: pluginManager,
            pluginOptions: pluginOptions,
        };
        if (pluginOptions && pluginOptions['subpage']) {
            switch (pluginOptions['subpage']) {
            case 'add-printer':
                page = Qt.resolvedUrl("AddPrinter.qml");
                break;
            case 'view-printer':
                var printerName = pluginOptions['name'];
                if (printerName) {
                    Printers.loadPrinter(printerName);
                    for (var i = 0; i < allPrintersList.count; i++) {
                        if (printerName == Printers.allPrinters.get(i).name) {
                            allPrintersList.currentIndex = i;
                            allPrintersList.currentItem.clicked();
                            break;
                        }
                    }
                }
                break;
            }
        }
        if (page) {
            pageStack.addPageToNextColumn(root, page, opts);
        }
    }

    Flickable {
        id: printerList
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        clip: true

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }

            ListView {
                id: allPrintersList
                anchors { left: parent.left; right: parent.right }
                contentHeight: contentItem.childrenRect.height
                boundsBehavior: Flickable.StopAtBounds
                height: contentHeight
                model: Printers.localPrinters

                section.property: "default"
                section.delegate: SettingsItemTitle {
                    text: section === "true" ? i18n.tr("Default printer")
                                             : i18n.tr("Configured printers")
                }

                delegate: printerDelegate
            }

            SettingsItemTitle {
                anchors { left: parent.left; right: parent.right }
                text: i18n.tr("Discovered printers")
                visible: discoveredPrintersRepeater.count
            }

            Repeater {
                id: discoveredPrintersRepeater
                anchors { left: parent.left; right: parent.right }
                model: Printers.remotePrinters
                delegate: printerDelegate
            }
        }
    }

    Component {
        id: printerDelegate

        ListItem {
            height: modelLayout.height + (divider.visible ? divider.height : 0)
            leadingActions: ListItemActions {
                actions: [
                    Action {
                        text: i18n.tr("Delete")
                        iconName: "delete"
                        onTriggered: {
                            if (!Printers.removePrinter(model.name)) {
                                console.error('failed to remove printer', Printers.lastMessage);
                            }
                        }
                    }
                ]
            }

            trailingActions: ListItemActions {
               actions: Action {
                   text: i18n.tr("Set as Default")
                   iconName: model.default ? "starred" : "non-starred"
                   enabled: !model.default && !isRemote
                   onTriggered: Printers.defaultPrinterName = model.name
               }
            }

            ListItemLayout {
                id: modelLayout
                title.text: displayName
                anchors { left: parent.left; right: parent.right }

                Icon {
                    id: icon
                    width: height
                    height: units.gu(2.5)
                    name: printerEnabled ? "printer-symbolic" : "media-playback-pause"
                    SlotsLayout.position: SlotsLayout.First
                }

                ProgressionSlot {}
            }
            onClicked: {
                Printers.loadPrinter(model.name);
                pageStack.addPageToNextColumn(root, Qt.resolvedUrl("Printer.qml"), {
                    printer: model, pluginOptions: pluginOptions,
                    plugin: plugin
                });
            }
        }
    }

    Item {
        visible: allPrintersList.count == 0
        z: 1

        anchors.fill: parent

        Label {
            id: noPrintersLabel
            anchors.centerIn: parent
            text: i18n.tr("There are no printers configured yet.")
            color: theme.palette.normal.baseText
        }

        Icon {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: noPrintersLabel.bottom
                topMargin: units.gu(2)
            }
            name: "printer-symbolic"
            height: units.gu(6)
            width: height
            color: theme.palette.normal.base
        }
    }
}
