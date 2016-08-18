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

import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Components 1.3
import "../utilities.js" as Utilities

Row {

    id: row

    property int buttonWidth
    property var repeater

    signal enteredQueueMode ()
    signal leftQueueMode ()
    signal removeQueued ()

    states: [
        State {
            name: "noneQueued"
            PropertyChanges {
                target: add
                text: i18n.tr("Cancel")
                onClicked: {
                    parent.state = "";
                    leftQueueMode();
                }
            }
            PropertyChanges {
                target: queue
                enabled: false
                opacity: 0.75
                text: i18n.tr("No images selected")
            }
        },
        State {
            name: "someQueued"
            extend: "noneQueued"
            when: Utilities.getSelected(repeater) > 0
            PropertyChanges {
                target: queue
                enabled: true
                opacity: 1
                text: {
                    var count = Utilities.getSelected(repeater);
                    return i18n.tr("Remove %1 image", "Remove %1 images", count).arg(count)
                }
                onClicked: {
                    parent.state = "";
                    removeQueued();
                }
            }
        }
    ]

    Button {
        id: add
        action: selectPeer
        objectName: "addCustomBackgroundsButton"
        text: i18n.tr("Add an image…")
        width: buttonWidth
        anchors {
            verticalCenter: parent.verticalCenter
        }
    }

    Button {
        id: queue
        objectName: "removeCustomBackgroundsButton"
        text: i18n.tr("Remove images…")
        width: buttonWidth
        anchors {
            verticalCenter: parent.verticalCenter
        }
        enabled: repeater.model.length > 0
        onClicked: {
            parent.state = "noneQueued"
            enteredQueueMode();
        }
    }
}
