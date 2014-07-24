/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/
import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Column {

    anchors.left: parent.left
    anchors.right: parent.right

    ListItem.Standard {
        text: i18n.tr("Edit SIM Name")
    }

    ListItem.Standard {
        text: sim1.title
        progression: true
        onClicked: {

        }
    }

    ListItem.Standard {
        text: sim2.title
        progression: true
    }

    Item {

        id: editor
        height: childrenRect.height
        anchors {
            left: parent.left
            right: parent.right
        }

        TextField {
            id: field
            text: "sim foo"
            maximumLength: 30
            width: parent.width - units.gu(4)
            anchors {
                topMargin: units.gu(2)
                horizontalCenter: parent.horizontalCenter
            }
        }

    }
}
