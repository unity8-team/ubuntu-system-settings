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
            parent.visible = true;
        else
            parent.visible = false;
    }

    Component.onCompleted: UpdateManager.checkUpdates()

    Connections {
        id: updateManager
        objectName: "updateManager"
        target: UpdateManager
        onModelChanged: root._updatesRefresh()
        onUpdateAvailableFound: root._updatesRefresh()
        onUpdatesNotFound: root._updatesRefresh()
        onCheckFinished: root._updatesRefresh()
    }

    onClicked: main.loadPluginByName("system-update");
}
