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


ListItem.ItemSelector {
    objectName: "defaultSim"
    text: i18n.tr("For outgoing calls, use:")
    expanded: true
    model: ["Off", "sim1", "sim2"]
    delegate: OptionSelectorDelegate {
        objectName: "use" + modelData
        text: {
            var t;
            if (modelData === "sim1") {
                t = sim1.title
            } else if (modelData === "sim2") {
                t = sim2.title
            } else {
                t = i18n.tr(modelData);
            }
            return t;
        }
    }
    selectedIndex: [true, sim1.connMan.powered, sim2.connMan.powered].lastIndexOf(true)
    onDelegateClicked: {
        sim1.connMan.powered = (index === 1)
        sim2.connMan.powered = (index === 2)
    }
}
