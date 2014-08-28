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

    Column {
        anchors.fill: content

        Label {
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.Wrap
            text: i18n.tr("Please insert a SIM card before you continue.") +
                  "\n\n" +
                  i18n.tr("Without it, you won’t be able to make calls or use text messaging.") +
                  "\n"
        }

        Image {
            id: image
            source: "data/meet_ubuntu_simcard@30.png"
            height: units.gu(6.5)
            width: units.gu(9)
        }
    }

    Component {
        id: forwardButton
        LocalComponents.StackButton {
            text: i18n.tr("Skip")
            rightArrow: true
            onClicked: pageStack.next()
        }
    }
}
