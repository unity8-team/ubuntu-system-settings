/*
 * Copyright (C) 2013 Canonical, Ltd.
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
 */

import QtQuick 2.0
import Ubuntu.Components 0.1

UbuntuShape {
    id: root

    property alias source: mainImage.source
    property string altSource
    property bool enabled
    signal clicked

    anchors {
        top: welcomeLabel.bottom
        left: parent.left
        margins: units.gu(2)
    }

    height: parent.height * 0.4
    width: parent.width * 0.45

    enabled: true

    image: Image {
        id: mainImage
        height: parent.height
        width: parent.width
        fillMode: Image.PreserveAspectCrop
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }

    states: State {
        name: "disabled"
        when: !root.enabled
        PropertyChanges {
            target: root
            image: altImage
        }
        PropertyChanges {
            target: mouseArea
            enabled: false
        }
        PropertyChanges {
            target : mainImage
            source : altSource
        }
    }

    transitions: Transition {
        NumberAnimation {
            properties: "source"
            easing.type: Easing.InOutQuad
            duration: 1000
        }
    }
}
