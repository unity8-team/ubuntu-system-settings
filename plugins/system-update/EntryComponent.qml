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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Update 1.0

ListItem.Standard {
    id: root
    text: i18n.tr(model.displayName)
    objectName: "entryComponent-updates"
    iconSource: model.icon
    iconFrame: false
    progression: true

    property int updatesAvailable: 0

    Label {
        id: labelCount
        objectName: "labelCount"
        text: root.updatesAvailable
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
            rightMargin: units.gu(4)
        }
        verticalAlignment: Text.AlignVCenter

        // This can not be added in the list, it needs to be inside the label
        UpdateManager {
            id: updateManager
            objectName: "updateManager"

            Component.onCompleted: {
                model.visible = true;
                updateManager.checkUpdates();
            }

            onUpdateAvailableFound: {
                root.updatesAvailable = updateManager.model.length;
                if (root.updatesAvailable > 0) {
                    model.visible = true;
                }
            }
        }
    }

    onClicked: pageStack.push(pluginManager.getByName("system-update").pageComponent);
}
