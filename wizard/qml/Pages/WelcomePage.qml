/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.LanguagePlugin 1.0
import "qrc:/qml/Components" as LocalComponents

LocalComponents.Page {
    title: i18n.tr("Hello!")
    forwardButtonSourceComponent: forwardButton

    readonly property bool simCardPresent: false // TODO: implement this

    UbuntuLanguagePlugin {
        id: plugin
    }

    Item {
        id: content
        anchors {
            fill: parent
            topMargin: topMargin
            leftMargin: leftMargin
            rightMargin: rightMargin
            bottomMargin: bottomMargin
        }

        Column {
            id: column
            spacing: units.gu(2)

            Label {
                id: label
                width: content.width
                wrapMode: Text.WordWrap
                fontSize: "large"
                text: i18n.tr("Welcome to Ubuntu.") + "\n" +
                      i18n.tr("Let’s get started.")
            }

            OptionSelector {
                id: languageList
                text: i18n.tr("Select your language")
                model: plugin.languageNames
                selectedIndex: plugin.currentLanguage
                onSelectedIndexChanged: {
                    i18n.language = plugin.languageCodes[selectedIndex]
                    i18n.domain = i18n.domain
                }
                containerHeight: content.height - label.height
                                 - column.spacing - units.gu(4)
            }
        }
    }

    Component {
        id: forwardButton
        Button {
            text: i18n.tr("Start")
            onClicked: {
                plugin.currentLanguage = languageList.selectedIndex
                pageStack.push(Qt.resolvedUrl(simCardPresent
                                              ? "LocationPage.qml"
                                              : "SimCardPage.qml"))
            }
        }
    }
}
