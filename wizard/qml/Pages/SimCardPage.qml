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
    title: i18n.tr("No SIM card installed")
    forwardButtonSourceComponent: forwardButton

    Item {
        id: content
        anchors {
            fill: parent
            topMargin: topMargin
            rightMargin: rightMargin
            bottomMargin: bottomMargin
        }

        Image {
            id: image
            anchors {
                top: parent.top
                left: parent.left
                leftMargin: units.gu(2.5)
            }
            source: "data/meet_ubuntu_simcard@30.png"
            height: units.gu(9)
            width: units.gu(12.5)
        }

        Label {
            anchors {
                top: image.bottom
                left: parent.left
                topMargin: units.gu(2)
                leftMargin: leftMargin
            }
            width: content.width
            wrapMode: Text.WordWrap
            fontSize: "large"
            text: i18n.tr("You need a valid SIM to use telephone and SMS features.")
        }
    }

    Component {
        id: forwardButton
        Button {
            text: i18n.tr("Skip")
            onClicked: pageStack.push(Qt.resolvedUrl("WiFiPage.qml"))
        }
    }
}
