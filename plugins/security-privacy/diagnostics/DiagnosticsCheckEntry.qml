/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Evan Dandrea <evan.dandrea@canonical.com>
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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

ListItem.Base {
    id: listItem

    property string textEntry: "";
    property bool checked: false

    Row {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        spacing: units.gu(2)

        CheckBox {
            id: checkBox

            property bool enableCheckConnection: true

            anchors.verticalCenter: parent.verticalCenter

            Component.onCompleted: {
                enableCheckConnection = false;
                checked = listItem.checked;
                enableCheckConnection = true;
            }

            // FIXME : should use Checkbox.toggled signal
            // lp:~nick-dedekind/ubuntu-ui-toolkit/checkbox.toggled
            onCheckedChanged: {
                if (!enableCheckConnection) return;

                var oldEnable = enableCheckConnection;
                enableCheckConnection = false;

                listItem.checked = checked;
                listItem.triggered(listItem.checked);

                enableCheckConnection = oldEnable;
            }

            Connections {
                target: listItem
                onCheckedChanged: {
                    if (!checkBox.enableCheckConnection) return;

                    var oldEnable = checkBox.enableCheckConnection;
                    checkBox.enableCheckConnection = false;

                    checkBox.checked = listItem.checked;

                    checkBox.enableCheckConnection = oldEnable;
                }
            }

            Connections {
                target: listItem.__mouseArea
                onClicked: checkBox.clicked();
            }
        }
        Label {
            anchors.verticalCenter: parent.verticalCenter
            text: textEntry
        }
    }
}
