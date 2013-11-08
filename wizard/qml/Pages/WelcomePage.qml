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
import "../Components" as LocalComponents

LocalComponents.Page {
    title: i18n.tr("Hello!")
    forwardButtonSourceComponent: forwardButton

    readonly property bool __simCardPresent: false // TODO: implement this

    UbuntuLanguagePlugin {
        id: plugin
    }

    Item {
        id: content
        anchors {
            fill: parent
            topMargin: __topMargin
            leftMargin: __leftMargin
            rightMargin: __rightMargin
            bottomMargin: __bottomMargin
        }

        Column {
            id: column
            spacing: units.gu(2)

            Label {
                id: label
                width: content.width
                wrapMode: Text.WordWrap
                fontSize: "large"
                text: i18n.tr("Welcome to Ubuntu.") + "\n" + i18n.tr("Let’s get started.")
            }

            OptionSelector {
                id: languageList
                text: i18n.tr("Select your language")
                model: plugin.languages
                selectedIndex: plugin.currentLanguage
                containerHeight: content.height - label.height - column.spacing - units.gu(4)
            }
        }
    }

    Component {
        id: forwardButton
        Button {
            text: i18n.tr("Start")
            onClicked: {
                plugin.currentLanguage = languageList.selectedIndex
                // TODO: Should go to WiFiPage.qml when that works
                pageStack.push(Qt.resolvedUrl(__simCardPresent ? "SpinnerPage.qml" : "SimCardPage.qml"))
            }
        }
    }
}
