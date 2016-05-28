/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/
import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.SystemSettings.Cellular 1.0

Column {

    id: defaultSim

    property var m: ["ask", sims[0].path, sims[1].path]

    function getNameFromIndex (index) {
        return [i18n.tr("Ask me each time"), sims[0].title, sims[1].title][index];
    }

    UbuntuCellularPanel {
        id: phoneSettings
    }

    SettingsItemTitle { text: i18n.tr("For outgoing calls, use:") }

    ListItem.ItemSelector {
        id: callsDefaultSim
        expanded: true
        model: m
        delegate: OptionSelectorDelegate {
            objectName: "defaultForCalls" + modelData
            text: getNameFromIndex(index);
        }
        selectedIndex: m.indexOf(phoneSettings.defaultSimForCalls)
        onDelegateClicked: {
            phoneSettings.defaultSimForCalls = m[index];
        }
    }

    ListItem.Caption {
        text: i18n.tr("You can change the SIM for individual calls, or for contacts in the address book.")
    }

    ListItem.Divider {}

    SettingsItemTitle { text: i18n.tr("For messages, use:") }

    ListItem.ItemSelector {
        id: messagesDefaultSim
        expanded: true
        model: m
        delegate: OptionSelectorDelegate {
            objectName: "defaultForMessages" + modelData
            text: getNameFromIndex(index);
        }
        selectedIndex: m.indexOf(phoneSettings.defaultSimForMessages)
        onDelegateClicked: {
            phoneSettings.defaultSimForMessages = m[index];

        }
    }

    Connections {
        target: Qt.application
        onStateChanged: {
            /* Set the selected index of selectors for both
            calls and SMS defaults upon regaining status as the top-most,
            focused window */
            if (state ===  Qt.ApplicationActive) {
                callsDefaultSim.selectedIndex =
                    defaultSim.m.indexOf(phoneSettings.defaultSimForCalls);

                messagesDefaultSim.selectedIndex =
                    defaultSim.m.indexOf(phoneSettings.defaultSimForMessages);
            }
        }
    }
}

