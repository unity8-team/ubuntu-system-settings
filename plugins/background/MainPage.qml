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

    property string homeBackground: background.pictureUri
    property string welcomeBackground: backgroundPanel.backgroundFile
    property real swappableWidth: mainPage.width * 0.43
    property real swappableHeight: mainPage.height * 0.4

    UbuntuBackgroundPanel {
        id: backgroundPanel

        function maybeUpdateSource() {
            var source = backgroundPanel.backgroundFile
            if (source != "" && source != undefined) {
                testWelcomeImage.source = source;
            }
            if (testWelcomeImage.source == "") {
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

    Column {
        id: previewsRow
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: spacing
        }
        height: childrenRect.height
        spacing: units.gu(2)

        Item {
            anchors.horizontalCenter: parent.horizontalCenter
            height: thumbRow.height
            width: thumbRow.width
            Row {
                id: thumbRow
                spacing: units.gu(2)
                height: childrenRect.height
                width: childrenRect.width

                Item {
                    anchors.top: parent.top
                    height: childrenRect.height
                    width: swappableWidth

                    SwappableImage {
                        id: welcomeImage
                        anchors.top: parent.top
                        height: swappableHeight
                        width: swappableWidth
                        onClicked: {
                            pageStack.push(Qt.resolvedUrl("Wallpapers.qml"),
                                           {homeScreen: systemSettingsSettings.backgroundDuplicate ? true : false,
                                               useSame: systemSettingsSettings.backgroundDuplicate,
                                               backgroundPanel: backgroundPanel,
                                               current: welcomeBackground
                                            });

                            var curItem = pageStack.currentPage;
                            selectedItemConnection.target = curItem;
                            updateImage(testWelcomeImage, welcomeImage);
                        }

                        Component.onCompleted: updateImage(testWelcomeImage,
                                                           welcomeImage)

                        OverlayImage {
                            anchors.fill: parent
                            source: "welcomeoverlay.svg"
                        }
                    }
                    Label {
                        id: welcomeLabel

                        anchors {
                            topMargin: units.gu(1)
                            top: welcomeImage.bottom
                            horizontalCenter: parent.horizontalCenter
                        }
                        text: i18n.tr("Welcome screen")
                    }
                }

                Item {
                    anchors.top: parent.top
                    height: childrenRect.height
                    width: swappableWidth
                    SwappableImage {
                        id: homeImage
                        anchors.top: parent.top
                        height: swappableHeight
                        width: swappableWidth

                        onClicked: {
                            pageStack.push(Qt.resolvedUrl("Wallpapers.qml"),
                                           {homeScreen: true,
                                               useSame: systemSettingsSettings.backgroundDuplicate,
                                               backgroundPanel: backgroundPanel,
                                               current: homeBackground
                                            });
                            var curItem = pageStack.currentPage;
                            selectedItemConnection.target = curItem;
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
                            horizontalCenter: parent.horizontalCenter
                        }

                        text: i18n.tr("Home screen")
                    }
                }

            }
            MouseArea {
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                height: swappableHeight
                visible: systemSettingsSettings.backgroundDuplicate
                onClicked: {
                    homeImage.clicked();
                }
            }
        }

        ListItem.ThinDivider {}

        OptionSelector {
            id: optionSelector
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - units.gu(4)
            expanded: true

            model: [i18n.tr("Same background for both"),
                i18n.tr("Different background for each")]
            selectedIndex: systemSettingsSettings.backgroundDuplicate ? 0 : 1
            onSelectedIndexChanged: {
                if (selectedIndex === 0 && !systemSettingsSettings.backgroundDuplicate)
                    systemSettingsSettings.backgroundDuplicate = true;
                else if (selectedIndex === 1 && systemSettingsSettings.backgroundDuplicate)
                    systemSettingsSettings.backgroundDuplicate = false;
            }
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
            // Will update source
            Utilities.updateWelcome(uri)
        }

        property string fallback: defaultBackground
        visible: false
        onStatusChanged: updateImage(testWelcomeImage, welcomeImage)
    }

    Image {
        id: testHomeImage

        function update(uri) {
            // Will update source
            Utilities.updateHome(uri)
        }

        property string fallback: defaultBackground
        source: background.pictureUri
        visible: false
        onStatusChanged: updateImage(testHomeImage, homeImage)
    }

    function setUpImages() {
        var mostRecent = (systemSettingsSettings.backgroundSetLast === "home") ?
                    testHomeImage : testWelcomeImage;
        var leastRecent = (mostRecent === testHomeImage) ?
                    testWelcomeImage : testHomeImage;

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
    }

    GSettings {
        id: systemSettingsSettings
        schema.id: "com.ubuntu.touch.system-settings"
        onChanged: {
            if (key == "backgroundDuplicate") {
                setUpImages()
            }
        }
        Component.onCompleted: {
            if (systemSettingsSettings.backgroundDuplicate)
                optionSelector.selectedIndex = 0;
            else
                optionSelector.selectedIndex = 1;
        }
    }

    Connections {
        id: selectedItemConnection
        onSave: {
            Utilities.setBackground(homeScreen, uri);
        }
    }
}
