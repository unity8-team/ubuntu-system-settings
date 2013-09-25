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
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.LanguagePlugin 1.0

SheetBase {
    id: root

    property string initialLanguage

    modal: true
    title: i18n.tr("Display language")

    contentsWidth: parent.width
    contentsHeight: parent.height

    Component.onCompleted: {
        initialLanguage = i18n.language
    }

    UbuntuLanguagePlugin {
        id: plugin
    }

    ListView {
        id: languageList

        clip: true

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: divider.top

        contentHeight: contentItem.childrenRect.height
        boundsBehavior: contentHeight > root.height ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        currentIndex: plugin.currentLanguage

        model: plugin.languages
        delegate: ListItem.Empty {
            Label {
                text: modelData

                anchors.left: parent.left
                anchors.right: checkMark.left
                anchors.leftMargin: units.gu(2)
                anchors.rightMargin: units.gu(2)
                anchors.verticalCenter: parent.verticalCenter
            }

            Image {
                id: checkMark

                /* Temporary until ItemSelector works again. */
                source: "/usr/lib/arm-linux-gnueabihf/qt5/qml/Ubuntu/Components/artwork/tick@30.png"

                opacity: index == languageList.currentIndex ? 1.0 : 0.0

                width: units.gu(2)
                height: units.gu(2)

                anchors.right: parent.right
                anchors.rightMargin: units.gu(2)
                anchors.verticalCenter: parent.verticalCenter
            }

            onClicked: {
                languageList.currentIndex = index
            }
        }

        onCurrentIndexChanged: {
            i18n.language = plugin.languageCodes[currentIndex]
            i18n.domain = i18n.domain
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

            text: i18n.tr("Cancel")

            anchors.left: parent.left
            anchors.right: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.topMargin: units.gu(1)
            anchors.leftMargin: units.gu(2)
            anchors.rightMargin: units.gu(1)
            anchors.bottomMargin: units.gu(1)

            onClicked: {
                i18n.language = initialLanguage
                i18n.domain = i18n.domain
                PopupUtils.close(root)
            }
        }

        Button {
            id: confirmButton

            text: i18n.tr("Confirm")

            anchors.left: parent.horizontalCenter
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.topMargin: units.gu(1)
            anchors.leftMargin: units.gu(1)
            anchors.rightMargin: units.gu(2)
            anchors.bottomMargin: units.gu(1)

            onClicked: {
                plugin.currentLanguage = languageList.currentIndex
                PopupUtils.close(root)
            }
        }
    }
}
