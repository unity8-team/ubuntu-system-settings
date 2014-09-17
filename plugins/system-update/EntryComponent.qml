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

    value: updatesAvailable > 0 ? updatesAvailable : ""

    property int updatesAvailable: 0
    property variant updateModel: UpdateManager.model

    function _updatesRefresh() {
        var _updatesAvailable = 0;
        for (var i=0; i < updateModel.length; i++) {
            if (updateModel[i].updateRequired)
                _updatesAvailable += 1;
        }
        root.updatesAvailable = _updatesAvailable;
        if (root.updatesAvailable > 0)
            root.parent.visible = true;
        else
            root.parent.visible = false;
    }

    Component.onCompleted: {
        // Ensure the entryComponent is hidden
        root.parent.visible = false;
        // TODO: Initiate a check on load, remove if timer gets enabled
        UpdateManager.checkUpdates();
    }

    Connections {
        id: updateManager
        objectName: "updateManager"
        target: UpdateManager
        onModelChanged: root._updatesRefresh()
    }

    onClicked: main.loadPluginByName("system-update");

    /* Don't check for updates immediately on startup */
    Timer {
        interval: 60000
        // TODO: Disable the timer until we decide when to check
        running: false
        repeat: false
        onTriggered: {
            /* Only check for updates if the model isn't already populated */
            if (root.updatesAvailable < 1)
                UpdateManager.checkUpdates();
        }
    }
}
