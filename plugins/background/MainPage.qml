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

    /* TODO: For now hardcoded paths, later we'll use GSettings */
    property string defaultBackground:
        mainPage.width >= units.gu(60) ?
            "/usr/share/unity8/graphics/tablet_background.jpg" :
            "/usr/share/unity8/graphics/phone_background.jpg"

    UbuntuBackgroundPanel {
        id: backgroundPanel

        function maybeUpdateSource() {
            console.log ("maybeUpdateSource: " + backgroundPanel.backgroundFile);
            var source = backgroundPanel.backgroundFile
            console.log ("SOURCE: " + source);
            if (source != "" && source != undefined) {
                console.log ("source is valid: " + source);
                testWelcomeImage.source = source;
            }
            if (testWelcomeImage.source == "") {
                console.log ("source isn't valid: " + testWelcomeImage.source);
                testWelcomeImage.source = testWelcomeImage.fallback
            }
        }

        onBackgroundFileChanged: maybeUpdateSource()
        Component.onCompleted: maybeUpdateSource()
    }

    GSettings {
        id: background
        schema.id: "org.gnome.desktop.background"
        onChanged: {
            if (key == "pictureUri")
                testHomeImage.source = value
        }
    }

    /* TODO: We hide the welcome screen parts for v1 -
       there's a lot of elements to hide */

    Label {
        id: welcomeLabel

        anchors {
            top: welcomeImage.bottom
            topMargin: units.gu(1)
            horizontalCenter: welcomeImage.horizontalCenter
        }

        text: i18n.tr("Welcome screen")
    }

    SwappableImage {
        id: welcomeImage

        anchors {
            top: parent.top
            left: parent.left
         }

        onClicked: {
            pageStack.push(Qt.resolvedUrl("Wallpapers.qml"),
                           {homeScreen: false,
                               backgroundPanel: backgroundPanel,
                               background: background,
                               systemSettingsSettings: systemSettingsSettings
                            });

            var curItem = pageStack.currentPage;
            curItemConnection.target = curItem;
            updateImage(testWelcomeImage, welcomeImage);
        }

        Component.onCompleted: updateImage(testWelcomeImage,
                                           welcomeImage)

        OverlayImage {
            anchors.fill: parent
            source: "welcomeoverlay.svg"
        }
    }

    SwappableImage {
        id: homeImage

        anchors {
            top: parent.top
            right: parent.right
         }

        onClicked: {
            pageStack.push(Qt.resolvedUrl("Wallpapers.qml"),
                           {homeScreen: true,
                               backgroundPanel: backgroundPanel,
                               background: background,
                               systemSettingsSettings: systemSettingsSettings
                            });
            var curItem = pageStack.currentPage;
            curItemConnection.target = curItem;
            updateImage(testHomeImage, homeImage);
        }
        Component.onCompleted: updateImage(testHomeImage,
                                           homeImage)

        OverlayImage {
            anchors.fill: parent
            source: "homeoverlay.svg"
        }
    }

    Label {
        id: homeLabel

        anchors {
            top: homeImage.bottom
            topMargin: units.gu(1)
            horizontalCenter: homeImage.horizontalCenter
        }

        text: i18n.tr("Home screen")
    }

    ListItem.ThinDivider {
        id: topDivider

        anchors {
            topMargin: units.gu(2)
            top: welcomeLabel.bottom
        }
    }

    OptionSelector {
        id: optionSelector
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: topDivider.bottom
            topMargin: units.gu(2)
        }
        width: parent.width - units.gu(4)
        expanded: true

        model: [i18n.tr("Same background for both"),
            i18n.tr("Different background for each")]
        //selectedIndex: systemSettingsSettings.backgroundDuplicate ? 0 : 1
        onSelectedIndexChanged: {
            if (selectedIndex === 0 && !systemSettingsSettings.backgroundDuplicate)
                systemSettingsSettings.backgroundDuplicate = true;
            else if (selectedIndex === 1 && systemSettingsSettings.backgroundDuplicate)
                systemSettingsSettings.backgroundDuplicate = false;
        }
    }

    Column {

        id: buttonColumn
        spacing: units.gu(1)

        anchors {
            topMargin: units.gu(2)
            left: parent.left
            right: parent.right
            top: optionSelector.bottom
        }

        Button {
            text: i18n.tr("Use original background")
            width: parent.width - units.gu(4)
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                // Reset all of the settings
                background.schema.reset('pictureUri')
                systemSettingsSettings.backgroundPreviouslySetValue =
                        background.pictureUri
                backgroundPanel.backgroundFile = background.pictureUri
                systemSettingsSettings.backgroundSetLast = "home"
                optionSelector.selectedIndex = 0 // Same
            }
        }
    }

    /* We don't have a good way of doing this after passing an invalid image to
       SwappableImage, so test the image is valid /before/ showing it and show a
       fallback if it isn't. */
    function updateImage(testImage, targetImage) {
        if (testImage.status === Image.Ready) {
            targetImage.source = testImage.source
        } else if (testImage.status === Image.Error) {
            targetImage.source = testImage.fallback
        }
    }

    Image {
        id: testWelcomeImage

        function update(uri) {
            console.log ("testWelcomeImage.update");
            // Will update source
            Utilities.updateWelcome(uri)
        }

        property string fallback: defaultBackground
        visible: false
        onStatusChanged: {
            console.log ("WELCOME onStatusChanged");
            console.log ("testWelcomeImage: "+ testWelcomeImage.source);
            console.log ("welcomeImage: "+ welcomeImage.source);
            updateImage(testWelcomeImage, welcomeImage);
        }
    }

    Image {
        id: testHomeImage

        function update(uri) {
            // Will update source
            console.log ("testHomeImage.update");
            Utilities.updateHome(uri)
        }

        property string fallback: defaultBackground
        source: background.pictureUri
        visible: false
        onStatusChanged: {
            console.log ("HOME onStatusChanged");
            console.log ("testHomeImage: "+ testHomeImage.source);
            console.log ("homeImage: "+ homeImage.source);
            updateImage(testHomeImage, homeImage);
        }
    }

    function setUpImages() {
        console.log ("setUpImages");
        console.log ("systemSettingsSettings.backgroundSetLast: " + systemSettingsSettings.backgroundSetLast);
        var mostRecent = (systemSettingsSettings.backgroundSetLast === "home") ?
                    testHomeImage : testWelcomeImage;
        var leastRecent = (mostRecent === testHomeImage) ?
                    testWelcomeImage : testHomeImage;

        console.log ("mostRecent.source: " + mostRecent.source);
        console.log ("leastRecent.source: " + leastRecent.source);
        if (systemSettingsSettings.backgroundDuplicate) { //same
            /* save value of least recently changed to restore later */
            systemSettingsSettings.backgroundPreviouslySetValue =
                    leastRecent.source
            /* copy most recently changed to least recently changed */
            leastRecent.update(mostRecent.source)
        } else { // different
            /* restore least recently changed to previous value */
            leastRecent.update(
                    systemSettingsSettings.backgroundPreviouslySetValue)
        }
        console.log ("mostRecent.source: " + mostRecent.source);
        console.log ("leastRecent.source: " + leastRecent.source);
    }

    GSettings {
        id: systemSettingsSettings
        schema.id: "com.ubuntu.touch.system-settings"
        onChanged: {
            console.log ("Changed: "+ key);
            if (key == "backgroundDuplicate") {
                setUpImages()
            }
        }
        Component.onCompleted: {
            if (systemSettingsSettings.backgroundDuplicate)
                optionSelector.selectedIndex = 0;
            else
                optionSelector.selectedIndex = 1;
            setUpImages();
        }
    }

    Connections {
        id: curItemConnection
        onSave: {
            console.log ("HERE: " + uri);
            console.log ("HERE homeScreen: " + homeScreen);

            Utilities.setBackground(homeScreen, uri);

        }
    }

    function selectImage(callback) {
        if (callback)
            console.log ("selectImage called");
    }
}
