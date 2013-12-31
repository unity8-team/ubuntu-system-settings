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
import Ubuntu.SystemSettings.Account 1.0
import "qrc:/qml/Components" as LocalComponents

LocalComponents.Page {
    title: i18n.tr("About you")
    forwardButtonSourceComponent: forwardButton

    UbuntuAccountPlugin {
        id: plugin
    }

    function advance() {
        plugin.accountRealName = textField.text
        pageStack.push(Qt.resolvedUrl("LocationPage.qml"))
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
            spacing: units.gu(2)

            Label {
                width: content.width
                wrapMode: Text.WordWrap
                fontSize: "large"
                text: i18n.tr("What shall we call you?")
            }

            TextField {
                id: textField
                width: content.width
                text: plugin.accountRealName
                placeholderText: "Your name"
                onAccepted: advance()
            }
        }
    }

    Component {
        id: forwardButton
        Button {
            text: i18n.tr("Continue")
            enabled: textField.text != ""
            onClicked: advance()
        }
    }
}
