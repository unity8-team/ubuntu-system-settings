/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2014 Canonical Ltd.
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
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.SystemSettings.Update 1.0


ItemPage {
    id: root
    objectName: "configurationPage"
    title: i18n.tr("Auto download")

    ListItem.ItemSelector {
        id: upgradePolicySelector
        expanded: true
        text: i18n.tr ("Download future updates automatically:")
        model: downloadSelector
        delegate: selectorDelegate
        selectedIndex: UpdateManager.downloadMode
        onSelectedIndexChanged: UpdateManager.downloadMode = selectedIndex
        Component.onCompleted: selectedIndex = UpdateManager.downloadMode
    }
    Component {
        id: selectorDelegate
        OptionSelectorDelegate { text: name; subText: description; }
    }

    ListModel {
        id: downloadSelector
        /* Workaround toolkit limitation, translated values can't be used
           to build listitem, so we don't it from js
           see https://bugreports.qt-project.org/browse/QTBUG-20631 */
        Component.onCompleted: {
            insert(0, { name: i18n.tr("Never"), description: "" })
            insert(1, { name: i18n.tr("When on wi-fi"), description: "" })
            insert(2, { name: i18n.tr("On any data connection"),
                       description: i18n.tr("Data charges may apply.") })
        }
    }
}
