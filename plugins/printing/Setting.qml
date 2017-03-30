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
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3

ItemPage {
    id: settingPage
    objectName: "settingPage"
    flickable: settingFlickable
    property var value

    Flickable {
        id: settingFlickable
        anchors.fill: parent

        Column {
            spacing: units.gu(2)
            anchors {
                top: parent.top
                topMargin: units.gu(2)
                left: parent.left
                right: parent.right
            }

            SettingsListItems.Standard {
                text: settingPage.title
                anchors {
                    left: parent.left
                    right: parent.right
                }

                TextField {
                    anchors {
                        left: parent.left
                        right: parent.right

                    }
                    text: settingPage.value
                    onTextChanged: settingPage.value = text
                }
            }
        }
    }
}
