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
    }

    function categoryToSelectorIndex(category) {
        switch (category) {
        case "policies": return 1;
        case "status": return 0;
        default: return 0;
        }
    }

    Flickable {
        id: printerFlickable
        anchors.fill: parent

        Loader {
            id: printerPageLoader
            anchors.fill: parent
            sourceComponent: printer.isLoaded ? printerLoadedComponent
                                              : printerLoadingComponent
        }
    }

    Component {
        id: printerLoadingComponent

        Item {
            anchors.fill: parent
            ActivityIndicator {
                anchors.centerIn: parent
                running: true
            }
        }
    }

    Component {
        id: printerLoadedComponent

        Column {
            spacing: units.gu(2)
            anchors {
                top: parent.top
                topMargin: units.gu(2)
                left: parent.left
                right: parent.right
            }

            OptionSelector {
                id: subPageSelector
                anchors {
                    left: parent.left
                    leftMargin: units.gu(2)
                }
                width: units.gu(30)
                model: [
                    i18n.tr("Printer status"),
                    i18n.tr("Policies"),
                    i18n.tr("Allowed users"),
                    i18n.tr("Installable options"),
                    i18n.tr("Copies and pages"),
                ]
                onSelectedIndexChanged: {
                    var uri = printerSubPageLoader.defaultUri;
                    switch (selectedIndex) {
                    case 1:
                        uri = Qt.resolvedUrl("Policies.qml");
                        break;
                    }
                    printerSubPageLoader.setSource(uri);
                }
            }

            Loader {
                id: printerSubPageLoader
                anchors { left: parent.left; right: parent.right }
                property string defaultUri: Qt.resolvedUrl("Status.qml")

                Component.onCompleted: {
                    // If we're asked for specific category, show it immediately.
                    var category = pluginOptions['category'];
                    var index = 0;
                    if (pluginOptions && category) {
                        index = categoryToSelectorIndex(category);
                    }

                    if (index == 0) {
                        printerSubPageLoader.setSource(defaultUri);
                    } else {
                        subPageSelector.selectedIndex = index;
                    }
                }
            }
        }
    }
}
