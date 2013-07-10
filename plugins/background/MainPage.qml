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
import GSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import SystemSettings 1.0
import Ubuntu.SystemSettings.Background 1.0

import "utilities.js" as Utilities

ItemPage {
    id: mainPage
    title: i18n.tr("Background")

    UbuntuBackgroundPanel {
        id: backgroundPanel

        function maybeUpdateSource() {
            var source = backgroundPanel.backgroundFile
            if (source != "" && source != undefined)
                testWelcomeImage.source = source
        }

        onBackgroundFileChanged: maybeUpdateSource()
        Component.onCompleted: maybeUpdateSource()
    }

    GSettings {
        id: background
        schema: "org.gnome.desktop.background"
    }

    Label {
        id: welcomeLabel

        anchors {
            top: parent.top
            topMargin: units.gu(1)
            horizontalCenter: welcomeImage.horizontalCenter
        }

        fontSize: "large"
        text: i18n.tr("Welcome screen")
    }

    SwappableImage {
        id: welcomeImage

        anchors {
            top: welcomeLabel.bottom
            left: parent.left
         }

        onClicked: pageStack.push(Utilities.createAlbumPage(
                                      i18n.tr("Welcome screen")))
    }

    Label {
        id: homeLabel

        anchors {
            top: parent.top
            topMargin: units.gu(1)
            horizontalCenter: homeImage.horizontalCenter
        }

        fontSize: "large"
        text: i18n.tr("Home screen")
    }

    SwappableImage {
        id: homeImage

        anchors {
            top: welcomeLabel.bottom
            right: parent.right
         }

        Component.onCompleted: updateImage(testHomeImage,
                                           homeImage)

        onClicked: pageStack.push(Utilities.createAlbumPage(
                                      i18n.tr("Home screen")))
    }

    /* We don't have a good way of doing this after passing an invalid image to
       SwappableImage, so test the image is valid /before/ showing it and show a
       fallback if it isn't. */
    function updateImage(testImage, targetImage) {
        // TODO: Doesn't yet fade when the background is changed, but probably
        // not a huge issue. Will resolve itself when we switch to the SDK's
        // CrossFadeImage
        if (testImage.status == Image.Ready) {
            targetImage.source = testImage.source
        } else if (testImage.status == Image.Error) {
            targetImage.source = testImage.fallback
        }
    }

    Image {
        id: testWelcomeImage
        property string fallback: "darkeningclockwork.jpg"
        visible: false
        onStatusChanged: updateImage(testWelcomeImage,
                                     welcomeImage)
    }

    Image {
        id: testHomeImage
        property string fallback: "aeg.jpg"
        source: background.pictureUri
        visible: false
        onStatusChanged: updateImage(testHomeImage,
                                     homeImage)
    }


    ListItem.ThinDivider {
        id: topDivider

        anchors {
            topMargin: units.gu(3)
            top: homeImage.bottom
        }
    }

    ListItem.Standard {
        id: sameBackground

        property string previousImage

        anchors.top: topDivider.bottom

        text: i18n.tr("Same background for both")

        selected: false

        showDivider: false

        // XXX: Ultimately this should all be done by states.
        // The current implementation is a demo.
        onClicked: {
            if (sameBackground.selected)
                return
            homeImage.altSource = welcomeImage.source
            homeImage.enabled = false
            differentBackground.selected = false
            sameBackground.selected = true
        }
    }

    ListItem.Standard {
        id: differentBackground

        anchors.top: sameBackground.bottom

        text: i18n.tr("Different background for each")

        selected: true

        onClicked: {
            if (differentBackground.selected)
                return
            homeImage.enabled = true
            differentBackground.selected = true
            sameBackground.selected = false
        }
    }
}
