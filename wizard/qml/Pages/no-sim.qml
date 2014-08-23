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
    title: i18n.tr("Add a SIM card")
    forwardButtonSourceComponent: forwardButton
    hasBackButton: false

    Item {
        id: content
        anchors {
            fill: parent
            topMargin: topMargin
            leftMargin: leftMargin
            rightMargin: rightMargin
            bottomMargin: bottomMargin
        }
 
        Image {
            id: image
            anchors {
                top: parent.top
                left: parent.left
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
            }

            width: parent.width
            wrapMode: Text.WordWrap
            textFormat: Text.StyledText
            text: i18n.tr("Please insert a SIM card before you continue. Without it, you won’t be able to make calls or use text messaging.")
        }
    }

    Component {
        id: forwardButton
        LocalComponents.ForwardButton {
            text: i18n.tr("Skip")
            onClicked: pageStack.next()
        }
    }
}
