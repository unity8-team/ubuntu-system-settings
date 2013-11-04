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
    readonly property real __buttonMargin: units.gu(2)
    readonly property real __buttonWidth: (width - __buttonMargin * 2) / 2 - __buttonMargin / 2
    readonly property real __topMargin: units.gu(5)
    readonly property real __leftMargin: units.gu(3)
    readonly property real __rightMargin: units.gu(3)
    readonly property real __bottomMargin: backButton.height + __buttonMargin * 2

    property bool hasBackButton: true
    property alias forwardButtonSourceComponent: forwardButton.sourceComponent

    visible: false

    Button {
        id: backButton
        width: __buttonWidth
        anchors {
            left: parent.left
            bottom: parent.bottom
            leftMargin: __buttonMargin
            bottomMargin: __buttonMargin
        }
        z: 1
        text: i18n.tr("Back")
        visible: pageStack.depth > 1 && hasBackButton
        gradient: UbuntuColors.greyGradient

        onClicked: pageStack.pop()
    }

    Loader {
        id: forwardButton
        width: __buttonWidth
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: __buttonMargin
            bottomMargin: __buttonMargin
        }
        z: 1
    }
}
