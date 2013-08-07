/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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
    title: i18n.tr("Carrier")
    property var netreg
    Component.onCompleted: console.log ("COUNT: " + netreg.operators.count())
    /*
    ActivityIndicator {
        id: activityIndicator
        anchors.centerIn: parent
        running: true
    }

    Text {
        anchors {
            top: activityIndicator.bottom
            topMargin: units.gu(2)
            horizontalCenter: activityIndicator.horizontalCenter
        }
        text: i18n.tr("Searching")
    }
    */
    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        Repeater {
            model: netreg.operators
            delegate: ListItem.SingleValue {
                text: modelData.name
            }
        }
    }
}
