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

Page {
    readonly property real buttonMargin: units.gu(2)
    readonly property real buttonWidth: (width - buttonMargin * 2) / 2 -
                                        buttonMargin / 2
    readonly property real topMargin: units.gu(5)
    readonly property real leftMargin: units.gu(3)
    readonly property real rightMargin: units.gu(3)
    readonly property real bottomMargin: backButton.height + buttonMargin * 3

    property bool hasBackButton: true
    property bool customBack: false
    property alias forwardButtonSourceComponent: forwardButton.sourceComponent

    signal backClicked()

    visible: false
    tools: ToolbarItems {
        back: null
    }

    Button {
        id: backButton
        width: buttonWidth
        anchors {
            left: parent.left
            bottom: parent.bottom
            leftMargin: buttonMargin
            bottomMargin: buttonMargin
        }
        z: 1
        text: i18n.tr("Back")
        visible: pageStack.depth > 1 && hasBackButton
        gradient: UbuntuColors.greyGradient

        onClicked: customBack ? backClicked() : pageStack.prev()
    }

    Loader {
        id: forwardButton
        width: buttonWidth
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: buttonMargin
            bottomMargin: buttonMargin
        }
        z: 1
    }
}
