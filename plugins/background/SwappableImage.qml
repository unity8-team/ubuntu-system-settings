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

Item {
    id: swappableImage

    /* Signals to connect through. See onCompleted of mouseArea for an example */
    signal clicked

    property string source
    property string altSource

    height: parent.height * 0.4
    width: parent.width * 0.43
    anchors.margins: units.gu(2)

    UbuntuShape {
        anchors.fill: parent
        image: ses
        onEnabledChanged: {
            /* Swap images */
            var tmpImage = source
            source = altSource
            altSource = tmpImage
        }
    }

    ShaderEffectSource {
        id: ses
        sourceItem: xfi
        width: 1
        height: 1
        hideSource: true
    }

    CrossFadeImage {
        id: xfi
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: swappableImage.source
        fadeDuration: UbuntuAnimation.SlowDuration
        visible: false
    }

    MouseArea {
        id: mouseArea
        anchors.fill: xfi
        Component.onCompleted: mouseArea.clicked.connect(swappableImage.clicked)
    }
}
