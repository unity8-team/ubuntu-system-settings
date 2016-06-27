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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.SystemSettings.Update 1.0

ListItem.SingleValue {
    id: root

    property int updatesAvailable: {
        var sUpdates = SystemImage.checkTarget() ? 1 : 0;
        var cUpdates = clickupdates.count;
        return sUpdates + cUpdates;
    }
    height: updatesAvailable > 0 ? units.gu(6) : 0

    text: i18n.tr(model.displayName)
    objectName: "entryComponent-updates"
    iconSource: Qt.resolvedUrl(model.icon)
    iconFrame: false
    progression: true
    value: updatesAvailable > 0 ? updatesAvailable : ""

    onClicked: main.loadPluginByName("system-update");

    UpdateModel {
        id: clickupdates
        filter: UpdateModel.PendingClicks
        Component.onCompleted: console.warn(UpdateModel.All, UpdateModel.PendingClicks)
    }

    Behavior on height { UbuntuNumberAnimation {} }
}
