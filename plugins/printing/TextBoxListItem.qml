/*
 * Copyright 2017 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by Jonas G. Drange <jonas.drange@canonical.com>
 */

import Ubuntu.Components 1.3

ListItem {
    id: root
    height: layout.height + (divider.visible ? divider.height : 0)
    property alias text: label.text
    property alias textFieldText: textField.text
    property alias field: textField
    property alias mainSlot: layout.mainSlot
    property alias placeholderText: textField.placeholderText
    property bool showDivider: true
    property bool enabled: true
    property bool highlightWhenPressed: true

    signal fieldTextChanged(string fieldText)

    divider.visible: showDivider
    divider.colorFrom: "#EEEEEE"
    divider.colorTo: "#EEEEEE"
    divider.height: units.dp(1)
    highlightColor: highlightWhenPressed ? undefined : "transparent"

    SlotsLayout {
        id: layout

        Label {
            id: label
            anchors.verticalCenter: parent.verticalCenter
            opacity: root.enabled ? 1 : 0.5
            width: implicitWidth > units.gu(10) ? implicitWidth : units.gu(10)
            SlotsLayout.position: SlotsLayout.First
            SlotsLayout.overrideVerticalPositioning: true
        }

        mainSlot: TextField {
            enabled: root.enabled
            id: textField
            onTextChanged: fieldTextChanged(text)
        }
    }
}
