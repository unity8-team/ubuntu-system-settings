/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Diego Sarmentero <diego.sarmentero@canonical.com>
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
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.SystemSettings.Update 1.0

ListItem.SingleValue {
    id: root
    text: i18n.tr(model.displayName)
    objectName: "entryComponent-updates"
    iconSource: Qt.resolvedUrl(model.icon)
    iconFrame: false
    progression: true

    value: root.updatesAvailable > 0 ? root.updatesAvailable : ""

    property int updatesAvailable: 0

    Item {
        UpdateManager {
            id: updateManager
            objectName: "updateManager"

            Component.onCompleted: {
                updateManager.checkUpdates();
            }

            onUpdateAvailableFound: {
                root.updatesAvailable = updateManager.model.length;
            }
        }
    }

    onClicked: main.loadPluginByName("system-update");
}
