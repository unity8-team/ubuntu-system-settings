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
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Column {

    property var m: ["ask", sim1.path, sim2.path]

    function getNameFromIndex (index) {
        return [i18n.tr("Ask me each time"), sim1.title, sim2.title][index];
    }

    ListItem.ItemSelector {
        id: callsDefaultSim
        text: i18n.tr("For outgoing calls, use:")
        expanded: true
        model: m
        delegate: OptionSelectorDelegate {
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

    ListItem.ItemSelector {
        id: messagesDefaultSim
        text: i18n.tr("For messages, use:")
        expanded: true
        model: m
        delegate: OptionSelectorDelegate {
            text: getNameFromIndex(index);
        }
        selectedIndex: m.indexOf(phoneSettings.defaultSimForMessages)
        onDelegateClicked: {
            phoneSettings.defaultSimForMessages = m[index];

        }
    }
}

