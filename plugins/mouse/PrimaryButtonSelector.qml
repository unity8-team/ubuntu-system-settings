/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015-2016 Canonical Ltd.
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
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

Column {
    property var selected

    Binding {
        target: primaryLeftCheck
        property: "checked"
        value: (selected === "left") ? true : false
    }

    Binding {
        target: primaryRightCheck
        property: "checked"
        value: (selected === "right") ? true : false
    }

    SettingsItemTitle {
        text: i18n.tr("Primary button:")
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
                            selected = "left"
                        else
                            checked = true;
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
                            selected = "right"
                        else
                            checked = true;
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
