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
import SystemSettings 1.0
import Ubuntu.Components 0.1
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
                gradient: UbuntuColors.greyGradient
                onClicked: {
                    parent.state = "";
                    leftQueueMode();
                }
            }
            PropertyChanges {
                target: queue
                enabled: false
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
                gradient: null
                text: {
                    var count = Utilities.getSelected(repeater);
                    return i18n.tr("Delete %1 image", "Delete %1 images", count).arg(count)
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
        action: selectDefaultPeer
        objectName: "addCustomBackgroundsButton"
        text: i18n.tr("Add an Image…")
        width: buttonWidth
        anchors {
            verticalCenter: parent.verticalCenter
        }
    }

    Button {
        id: queue
        gradient: UbuntuColors.greyGradient
        objectName: "removeCustomBackgroundsButton"
        text: i18n.tr("Remove Images…")
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
