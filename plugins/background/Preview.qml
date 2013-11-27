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
import GSettings 1.0
import Ubuntu.Components 0.1
import SystemSettings 1.0
import Ubuntu.SystemSettings.Background 1.0
import "utilities.js" as Utilities

ItemPage {
    id: preview
    anchors.fill: parent

    property string uri
    property bool homeScreen

    states: [
        State {
            name: "showing"
            StateChangeScript {
                script: {
                    pageStack.currentPage.header.opacity = 0.7;
                    pageStack.currentPage.toolbar.opacity = 0.7;
                }

            }
        },
        State {
            name: "destroyed"
            StateChangeScript {
                script: {
                    pageStack.currentPage.header.opacity = 1.0;
                    pageStack.currentPage.toolbar.opacity = 1.0;
                    pageStack.pop();
                }
            }
        }
    ]
    Component.onCompleted: {
        state = "showing";
        console.log ("homeScreen: " + homeScreen);
    }

    title: i18n.tr("Preview")

    GSettings {
        id: background
        schema.id: "org.gnome.desktop.background"
    }

    Action {
        id: cancelAction
        text: i18n.tr("Cancel")
        iconName: "back"
        onTriggered: {
            state = "destroyed";
        }
    }

    Action {
        id: setAction
        text: i18n.tr("Set")
        iconName: "import-image"
        onTriggered: {
            console.log ("Set wallpaper here");
            Utilities.setBackground(uri);
            state = "destroyed";
        }
    }

    tools: ToolbarItems {
        back: ToolbarButton {
            action: cancelAction
        }
        ToolbarButton {
            action: setAction
        }
    }

    UbuntuBackgroundPanel {
        id: backgroundPanel
    }

    GSettings {
        id: systemSettingsSettings
        schema.id: "com.ubuntu.touch.system-settings"
    }

    Image {
        id: previewImage
        anchors.centerIn: parent
        source: uri
        height: parent.height
        fillMode: Image.PreserveAspectFit
    }
}
