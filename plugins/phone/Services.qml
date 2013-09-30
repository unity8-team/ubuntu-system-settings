/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Sebastien Bacher <sebastien.bacher@canonical.com>
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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

ItemPage {
    property string carrierString
    property var sim
    // TRANSLATORS: %1 is the name of the (network) carrier
    title: i18n.tr("%1 Services").arg(carrierString)

    Column {
        anchors.left: parent.left
        anchors.right: parent.right
        Repeater {
            model: sim.serviceNumbers

            ListItem.Standard {
                progression: true
                text: modelData.name
                onClicked: pageStack.push(Qt.resolvedUrl("ServiceInfo.qml"), {serviceName: modelData.name, serviceNumber: modelData.value})
            }
        }
    }
}
