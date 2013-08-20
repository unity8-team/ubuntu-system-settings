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
import "../Components" as LocalComponents

LocalComponents.Page {
    title: i18n.tr("Your phone and location")
    forwardButtonSourceComponent: forwardButton

    Item {
        id: content
        anchors {
            fill: parent
            topMargin: __topMargin
            leftMargin: __leftMargin
            rightMargin: __rightMargin
            bottomMargin: __bottomMargin
        }

        Label {
            width: parent.width
            wrapMode: Text.WordWrap
            fontSize: "large"
            text: i18n.tr("Your phone is setup to automatically report errors to Canonical. This can be disabled in system settings.\n\nYour phone is setup to detect your location. This can be disabled in system settings.")
        }
    }

    Component {
        id: forwardButton
        Button {
            text: i18n.tr("Continue")
            onClicked: pageStack.push(Qt.resolvedUrl("SetupCompletePage.qml"))
        }
    }
}
