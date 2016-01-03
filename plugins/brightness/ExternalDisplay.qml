/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Jonas G. Drange <jonas.drange@canonical.com>
 */

import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Settings.Menus 0.1 as Menus
import Ubuntu.Settings.Components 0.1 as USC
import Ubuntu.SystemSettings.Brightness 1.0

Column {

    property var localOrientation
    property string localMode
    property var localEnabled

    signal apply();

    ListItems.Standard {
        text: i18n.tr("External display")
        enabled: model.connected
        onClicked: enabledCheck.trigger()
        control: CheckBox {
            id: enabledCheck
            property bool serverChecked: model.enabled
            onServerCheckedChanged: checked = serverChecked
            Component.onCompleted: checked = serverChecked
            onTriggered: {
                console.warn('disabling display');
                localEnabled = checked;
            }
        }
    }

    ListItems.SingleValue {
        text: i18n.tr("Rotation")
        value: {
            console.warn('display orientation', model.orientation);
            switch (model.orientation) {
                case Display.Normal:
                    return i18n.tr("None");
                    break;
                case Display.PortraitMode:
                    return i18n.tr("90° clockwise");
                    break;
                case Display.LandscapeInvertedMode:
                    return i18n.tr("180° clockwise");
                    break;
                case Display.PortraitInvertedMode:
                    return i18n.tr("270° clockwise");
                    break;
                default:
                    throw "Unable to determine orientation type.";
            }
        }
        visible: model.enabled
        progression: true
        onClicked: {
            var rotationPage = pageStack.push(
                Qt.resolvedUrl("PageRotation.qml"), {
                    orientation: model.orientation
                }
            );
            rotationPage.orientationChanged.connect(
                function () {
                    console.warn('locally setting orientation', rotationPage.orientation);
                    localOrientation = rotationPage.orientation;
                }
            );
        }
    }

    ListItems.SingleValue {
        text: i18n.tr("Resolution")
        value: model.mode
        visible: model.enabled
        progression: true
        onClicked: {
            var resPage = pageStack.push(
                Qt.resolvedUrl("PageResolution.qml"), {
                    modes: availableModes,
                    mode: mode
                }
            );
            resPage.modeChanged.connect(
                function (mode) {
                    localMode = mode;
                }
            );
        }
    }

    SettingsItemTitle {
        text: i18n.tr("Scale UI elements")
        visible: model.enabled && showAllUI
        showDivider: false
    }

    Menus.SliderMenu {
        id: scaleSlider
        objectName: "scaleSlider"
        visible: model.enabled && showAllUI
        live: true
        minimumValue: 0.0
        maximumValue: 100.0
        value: localScale >= 0 ? localScale : model.scale
        onUpdated: localScale = value
    }

    Column {
        anchors {
            left: parent.left;
            right: parent.right
            leftMargin: spacing
            rightMargin: spacing
        }
        visible: model.enabled
        spacing: units.gu(1)

        Button {
            anchors { left: parent.left; right: parent.right }
            text: i18n.tr("Apply changes")
            enabled: localOrientation ||
                     localMode ||
                     (typeof localEnabled !== "undefined")
            onClicked: {
                if (localOrientation) {
                    model.orientation = localOrientation;
                    localOrientation = null;
                }

                if (localMode) {
                    model.mode = localMode;
                    localMode = "";
                }

                if (typeof localEnabled !== "undefined") {
                    model.enabled = localEnabled;
                    localEnabled = null;
                }

                apply();
            }
        }

        ListItems.ThinDivider { opacity: 0 }

        Button {
            anchors { left: parent.left; right: parent.right }
            text: i18n.tr("Sound settings…")
            onClicked: {
                var sPlugin = pluginManager.getByName("sound")
                if (sPlugin) {
                    var soundPage = sPlugin.pageComponent;
                    if (soundPage)
                        pageStack.push(soundPage);
                    else
                        console.warn(
                            "Failed to get sound page " +
                            "pageComponent"
                        );
                } else {
                    console.warn(
                        "Failed to get sound plugin " +
                        "instance"
                    )
                }
            }
        }

        Button {
            anchors { left: parent.left; right: parent.right }
            text: i18n.tr("External keyboard…")
        }
    }
}
