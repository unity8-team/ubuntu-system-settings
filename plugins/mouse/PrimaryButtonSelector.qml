/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Ken VanDine <ken.vandine@canonical.com>
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
import SystemSettings 1.0
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.2 as ListItem

Column {
    property var buttonEnum

    Binding {
        target: primaryLeftCheck
        property: "checked"
        value: (buttonEnum === "Left") ? true : false
    }

    Binding {
        target: primaryRightCheck
        property: "checked"
        value: (buttonEnum === "Right") ? true : false
    }

    SettingsItemTitle {
        text: i18n.tr("Primary button:")
        showDivider: false
    }

    ListItem.Empty {
        anchors { 
            left: parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        showDivider: false
        highlightWhenPressed: false
        height: itemRow.height
        Row {
            id: itemRow
            spacing: units.gu(2)
            Row {
                spacing: units.gu(1)
                CheckBox {
                    id: primaryLeftCheck
                    onTriggered: {
                        if (checked)
                            buttonEnum = "Left"
                    }
                }
                Label {
                    height: primaryLeftCheck.height
                    verticalAlignment: Text.AlignVCenter
                    text: i18n.tr("Left")
                }
            }
            Row {
                spacing: units.gu(1)
                CheckBox {
                    id: primaryRightCheck
                    onTriggered: {
                        if (checked)
                            buttonEnum = "Right"
                    }
                }
                Label {
                    height: primaryRightCheck.height
                    verticalAlignment: Text.AlignVCenter
                    text: i18n.tr("Right")
                }
            }
        }
    }
}
