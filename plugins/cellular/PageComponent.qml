/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

ItemPage {
    title: i18n.tr("Cellular")

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.ValueSelector {
            id: chooseCarrier
            expanded: true
            // TODO: There is no way to have a ValueSelector always expanded
            onExpandedChanged: expanded = true
            text: i18n.tr("Choose carrier:")
            values: [i18n.tr("Automatically"), i18n.tr("Manually")]
        }

        ListItem.SingleValue {
            text: i18n.tr("Carrier")
            value: "Aubergine"
            property bool enabled: chooseCarrier.selectedIndex == 1 // Manually
            progression: enabled
            onClicked: {
                if (enabled)
                    pageStack.push(Qt.resolvedUrl("ChooseCarrier.qml"))
            }
        }

        ListItem.Divider {}

        ListItem.Standard {
            text: i18n.tr("Cellular data")
            control: Switch {
                id: cellularDataControl
                checked: false
            }
        }

        ListItem.ValueSelector {
            id: dataTypeSelector
            expanded: true
            // TODO: There is no way to have a ValueSelector always expanded
            onExpandedChanged: expanded = true
            enabled: cellularDataControl.checked
            values: [i18n.tr("2G only (saves battery)"),
                i18n.tr("2G/3G/4G (faster)")]
        }

        ListItem.Standard {
            text: i18n.tr("Call roaming")
            control: Switch {
                id: callRoamingControl
                checked: true
            }
        }

        ListItem.Standard {
            text: i18n.tr("Data roaming")
            control: Switch {
                id: dataRoamingControl
                checked: false
                onClicked: {
                    if (!callRoamingControl.checked ||
                            !cellularDataControl.checked)
                        checked = false
                }
                Connections {
                    target: callRoamingControl
                    onCheckedChanged: {
                        //TODO: Should recall previous state if true
                        if (!callRoamingControl.checked)
                            dataRoamingControl.checked = false
                    }
                }
                Connections {
                    target: cellularDataControl
                    onCheckedChanged: {
                        //TODO: Should recall previous state if true
                        if (!cellularDataControl.checked)
                            dataRoamingControl.checked = false
                    }
                }
            }
        }

        ListItem.Divider {}

        ListItem.Standard {
            text: i18n.tr("Data usage statistics")
            progression: true
        }
    }
}
