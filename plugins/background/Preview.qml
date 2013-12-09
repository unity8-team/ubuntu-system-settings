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
 * Ken VanDine <ken.vandine@canonical.com>
 *
*/

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

ItemPage {
    id: preview
    anchors.fill: parent

    property string uri
    signal save

    tools: null
    Component.onCompleted: {
        /* change the header text color to make it more readable over the background */
        header.__styleInstance.textColor = Theme.palette.selected.foregroundText;
    }

    Component.onDestruction: {
        header.__styleInstance.textColor = Theme.palette.selected.backgroundText;
    }

    states: [
        State {
            name: "saved"
            StateChangeScript {
                script: {
                    save();
                    pageStack.pop();
                }
            }
        },
        State {
            name: "cancelled"
            StateChangeScript {
                script: {
                    pageStack.pop();
                }
            }
        }
    ]

    title: i18n.tr("Preview")

    Image {
        id: previewImage
        anchors.centerIn: parent
        source: uri
        height: parent.height
        fillMode: Image.PreserveAspectFit
    }

    ListItem.ThinDivider {
        anchors.bottom: previewButtons.top
        anchors.bottomMargin: units.gu(1)
    }

    ListItem.Base {
        id: previewButtons
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        showDivider: false

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: units.gu(2)

            Button {
                text: i18n.tr("Cancel")
                width: (previewButtons.width-units.gu(2)*4)/2
                gradient: UbuntuColors.greyGradient
                onClicked: preview.state = "cancelled"
            }
            Button {
                text: i18n.tr("Set")
                width: (previewButtons.width-units.gu(2)*4)/2
                onClicked: preview.state = "saved"
            }
        }
    }

    /* Make the header slightly darker to ease readability on light backgrounds */
    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        color: "black"
        opacity: 0.3
        height: preview.header.height
    }
}
