/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2014 Canonical Ltd.
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

ListItem.Standard {
    id: root
    text: i18n.tr("Updates Available")
    icon: Qt.resolvedUrl("file:///usr/share/ubuntu/settings/system/icons/distributor-logo.png")
    iconFrame: false
    progression: true
    visible: root.updatesAvailable == 0 ? false : true

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
    }
}
