/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
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
 */

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.LanguagePlugin 1.0

ItemPage {
    id: root

    title: i18n.tr("Display language")

    UbuntuLanguagePlugin {
        id: plugin
    }

    Flickable {
        clip: true

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: divider.top

        contentHeight: contentItem.childrenRect.height

        ListItem.ValueSelector {
            id: languageList

            text: i18n.tr("Display Language")
            values: plugin.languages
            selectedIndex: plugin.currentLanguage

            expanded: true
            onExpandedChanged: expanded = true
        }
    }

    ListItem.ThinDivider {
        id: divider

        anchors.bottom: buttonRectangle.top
    }

    Item {
        id: buttonRectangle

        height: cancelButton.height + units.gu(2)

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Button {
            id: cancelButton

            anchors.left: parent.left
            anchors.right: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.topMargin: units.gu(1)
            anchors.leftMargin: units.gu(2)
            anchors.rightMargin: units.gu(1)
            anchors.bottomMargin: units.gu(1)

            text: i18n.tr("Cancel")
            onClicked: pageStack.pop()
        }

        Button {
            id: confirmButton

            anchors.left: parent.horizontalCenter
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.topMargin: units.gu(1)
            anchors.leftMargin: units.gu(1)
            anchors.rightMargin: units.gu(2)
            anchors.bottomMargin: units.gu(1)

            text: i18n.tr("Confirm")
            onClicked: {
                plugin.currentLanguage = languageList.selectedIndex
                pageStack.pop()
            }
        }
    }
}
