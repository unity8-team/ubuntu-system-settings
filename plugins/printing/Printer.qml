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
        extension: Label {
            text: printer.description
            anchors {
                left: parent.left
                leftMargin: units.gu(5)
                bottom: parent.bottom
                bottomMargin: units.gu(1)
            }
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

            SettingsListItems.StandardProgression {
                text: i18n.tr("General settings")
                onClicked: pageStack.addPageToNextColumn(
                    printerPage, Qt.resolvedUrl("GeneralSettings.qml"),
                    { printer: printer }
                )
            }

            SettingsListItems.StandardProgression {
                text: i18n.tr("Policies")
                onClicked: pageStack.addPageToNextColumn(
                    printerPage, Qt.resolvedUrl("Policies.qml"),
                    { printer: printer }
                )
            }
        }
    }
}
