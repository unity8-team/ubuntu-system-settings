/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
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
 * Iain Lane <iain.lane@canonical.com>
 *
 */

import QtQuick 2.0

import Ubuntu.Components 0.1

import Ubuntu.Components.ListItems 0.1

import SystemSettings 1.0

ItemPage {

    id: rootPage

    ListModel {
        id : galleriesModel

        ListElement {
            name : "Ubuntu Art"
        }

        ListElement {
            name : "Camera Photos"
        }

        ListElement {
            name : "An Album"
        }

        ListElement {
            name : "Another Album"
        }

    }

    ListView {
        anchors.fill: parent
        model : galleriesModel

        delegate: Standard {
            text : name
            icon : Qt.resolvedUrl("greenplant.jpg")
            progression : true
        }
    }

}
