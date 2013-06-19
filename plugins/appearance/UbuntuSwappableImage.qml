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

Item {

    id: root

    state: "ubuntu"

    property alias source : img.source
    property alias altSource : img2.source
    property alias duration : animation.duration

    /* Signals to connect through. See onCompleted of mouseArea for an example */
    signal clicked

    function swapImage() {
        state = state == "one" ? "two" : "one"
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        Component.onCompleted: {
            mouseArea.clicked.connect(root.clicked)
        }
    }

    UbuntuShape {
        id: shape

        anchors.fill: parent

        image: Image {
            id: img
            fillMode: Image.PreserveAspectCrop
        }
    }
    UbuntuShape {
        id: shape2
        anchors.fill: shape

        opacity: 0

        image: Image {
            id: img2
            fillMode: Image.PreserveAspectCrop
        }
    }
    states: [
        State {
            name: "one"

            PropertyChanges {
                target: shape2
                opacity: 1
            }
            PropertyChanges {
                target: shape
                opacity: 0
            }
        },
        State {
            name: "two"

            PropertyChanges {
                target: shape
                opacity: 1
            }
            PropertyChanges {
                target: shape2
                opacity: 0
            }
        }
    ]
    transitions: Transition {
        NumberAnimation {
            id : animation

            duration : 1000
            properties: "opacity"
            easing.type: Easing.InOutQuad
        }
    }
}
