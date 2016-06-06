/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Themes.Ambiance 1.3
import Ubuntu.Keyboard 0.1
import SystemSettings 1.0

TextField {
    id: field
    property var next
    anchors {
        left: parent.left
        right: parent.right
    }
    height: implicitHeight + units.gu(2)
    style: TextFieldStyle {
        overlaySpacing: units.gu(1)
        frameSpacing: units.gu(1)
        background: Rectangle {
            property bool error: (field.hasOwnProperty("errorHighlight") &&
                                 field.errorHighlight &&
                                 !field.acceptableInput)
            onErrorChanged: error ? theme.palette.normal.negative : color
            color: Theme.palette.selected.background
            anchors.fill: parent
            visible: field.activeFocus
        }
        color: UbuntuColors.lightAubergine
    }

    // Ubuntu.Keyboard
    // TRANSLATORS: This is the text that will be used on the "return" key for the virtual keyboard,
    // this word must be less than 5 characters
    InputMethod.extensions: { "enterKeyText": i18n.tr("Next") }
    KeyNavigation.tab: next
    Keys.onReturnPressed: next.forceActiveFocus()
}
