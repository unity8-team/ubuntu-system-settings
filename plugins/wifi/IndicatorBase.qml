/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
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
 *      Nick Dedekind <nick.dedekind@canonical.com>
 */

import QtQuick 2.4
import QMenuModel 0.1 as QMenuModel
import SystemSettings 1.0
import Ubuntu.Components 1.3

ItemPage {
    id: indicatorItem

    // FIXME : should be disabled until bus available when we have desktop indicators
    // for now, disable when we dont habe the correct profile.
    enabled: menuObjectPaths.hasOwnProperty(device)

    //const
    property string title
    property string busName
    property string actionsObjectPath
    property var menuObjectPaths: undefined
    readonly property string device: "phone_wifi_settings"
    property string rootMenuType: "com.canonical.indicator.root"
    property bool active: false

    property string deviceMenuObjectPath: menuObjectPaths.hasOwnProperty(device) ? menuObjectPaths[device] : ""

    signal actionGroupUpdated()
    signal modelUpdated()
}
