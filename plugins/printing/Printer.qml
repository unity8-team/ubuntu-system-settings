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
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3

ItemPage {
    id: printerPage
    objectName: "printerPage"
    property var printer
    header: PageHeader {
        title: printer.name
        flickable: printerFlickable
        extension: Loader {
            active: printer.isLoaded
            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: units.gu(2)
                right: parent.right
                rightMargin: units.gu(2)
            }
            sourceComponent: Sections {
                model: [
                    i18n.tr("Printer status"),
                    i18n.tr("General settings"),
                    i18n.tr("Policies"),
                    /* i18n.tr("Allowed users"), */
                    /* i18n.tr("Installable options"), */
                    i18n.tr("Copies and pages"),
                ]

                Component.onCompleted: {
                    // If we're asked for specific category, show it immediately.
                    if (pluginOptions && pluginOptions["category"]) {
                        selectedIndex = categoryToSelectorIndex(pluginOptions["category"]);
                    }
                }
            }
        }
    }

    function categoryToSelectorIndex(category) {
        switch (category) {
        case "general": return 1;
        case "policies": return 2;
        case "copiesandpages": return 3;
        case "status": return 0;
        default: return 0;
        }
    }

    Flickable {
        id: printerFlickable
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height

        Loader {
            id: printerPageLoader
            anchors {
                left: parent.left
                right: parent.right
            }
            sourceComponent: printer.isLoaded ? printerLoadedComponent
                                              : printerLoadingComponent
        }
    }

    Component {
        id: printerLoadingComponent

        Item {
            ActivityIndicator {
                anchors {
                    top: parent.top
                    topMargin: units.gu(2)
                    horizontalCenter: parent.horizontalCenter
                }
                running: true
            }
        }
    }

    Component {
        id: printerLoadedComponent

        Column {
            spacing: units.gu(2)
            height: childrenRect.height + anchors.topMargin
            anchors {
                top: parent.top
                topMargin: units.gu(2)
                left: parent.left
                right: parent.right
            }

            Loader {
                id: printerSubPageLoader
                anchors { left: parent.left; right: parent.right }
                source: {
                    if (printerPage.header.extension.item) {
                        switch (printerPage.header.extension.item.selectedIndex) {
                        case 1:
                            Qt.resolvedUrl("GeneralSettings.qml");
                            break;
                        case 2:
                            Qt.resolvedUrl("Policies.qml");
                            break;
                        case 3:
                            Qt.resolvedUrl("CopiesAndPages.qml");
                            break;
                        case 0:
                        default:
                            Qt.resolvedUrl("Status.qml");
                            break;
                        }
                    } else {
                        null
                    }
                }
            }
        }
    }
}
