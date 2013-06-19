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

Item {
    property url source

    readonly property bool running: crossFadeImages.running || nextImageFadeIn.running
    readonly property size sourceSize: priv.currentImage.sourceSize
    readonly property var status: priv.currentImage ? priv.currentImage.status : Image.Null


    QtObject {
        id: priv

        /* \internal
          Defines the image currently being shown
          */
        property Image currentImage: image1
        /* \internal
          Defines the image being changed to
          */
        property Image nextImage: image2

    }

    function swapImages() {
        priv.currentImage.z = 0
        priv.nextImage.z = 1
        if (crossFade) {
            crossFadeImages.start();
        } else {
            nextImageFadeIn.start();
        }

        var tmpImage = priv.currentImage
        priv.currentImage = priv.nextImage
        priv.nextImage = tmpImage
    }

    onSourceChanged: {
        // On creation, the souce handler is called before image pointers are set.
        if (priv.currentImage === null) {
            priv.currentImage = image1;
            priv.nextImage = image2;
        }

        crossFadeImages.stop();
        nextImageFadeIn.stop();

        // Don't fade in initial picture, only fade changes
        if (priv.currentImage.source == "" && !fadeInFirst) {
            priv.currentImage.source = source;
        } else {
            nextImageFadeIn.stop();
            priv.nextImage.opacity = 0.0;
            priv.nextImage.source = source;

            // If case the image is still in QML's cache, status will be "Ready" immediately
            if (priv.nextImage.status === Image.Ready || priv.nextImage.source === "") {
                swapImages();
            }
        }
    }

    Connections {
        target: priv.nextImage
        onStatusChanged: {
            if (priv.nextImage.status == Image.Ready) {
                 swapImages();
             }
        }
    }

    Image {
        id: image1
        anchors.fill: parent
        cache: false
        asynchronous: true
        z: 1
    }

    Image {
        id: image2
        anchors.fill: parent
        cache: false
        asynchronous: true
        z: 0
    }

    NumberAnimation {
        id: nextImageFadeIn
        target: priv.nextImage
        property: "opacity"
        duration: 400
        to: 1.0
        easing.type: Easing.InOutQuad

        onRunningChanged: {
            if (!running) {
                priv.nextImage.source = "";
            }
        }
    }

    ParallelAnimation {
        id: crossFadeImages
        NumberAnimation {
            target: priv.nextImage
            property: "opacity"
            duration: 400
            to: 1.0
            easing.type: Easing.InOutQuad
        }

        NumberAnimation {
            target: priv.currentImage
            property: "opacity"
            duration: 400
            to: 0
            easing.type: Easing.InOutQuad
        }

        onRunningChanged: {
            if (!running) {
                priv.nextImage.source = "";
            }
        }
    }
}
