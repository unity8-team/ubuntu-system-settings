/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Sebastien Bacher <sebastien.bacher@canonical.com>
 *
 */

import GSettings 1.0
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import SystemSettings 1.0

ItemPage {
    id: dashPage
    title: i18n.tr("Dash search")

    GSettings {
        id: unitySettings
        schema.id: "com.canonical.Unity.Lenses"
        onChanged: {
            if (key == "remoteContentSearch")
                if (value == 'all')
                    selectorId.selectedIndex = 1
                else
                    selectorId.selectedIndex = 0
        }
    }

    ListItem.ItemSelector {
        id: selectorId
        text: i18n.tr("Return results from:")
        model: [i18n.tr("Phone only"), i18n.tr("Phone and Internet")]
        selectedIndex: (unitySettings.remoteContentSearch === 'all') ? 1 : 0
        expanded: true
        onSelectedIndexChanged:
            unitySettings.remoteContentSearch = (selectorId.selectedIndex == 0) ? "none" : "all"
    }
}
