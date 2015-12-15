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

    property var localOrientation: null
    property string localMode: ""
    property double localScale: -1

    ListItems.Standard {
        text: i18n.tr("External display")
        enabled: model.connected
        onClicked: enabledCheck.trigger()
        control: CheckBox {
            id: enabledCheck
            property bool serverChecked: model.enabled
            onServerCheckedChanged: checked = serverChecked
            Component.onCompleted: checked = serverChecked
            onTriggered: model.enabled = checked
        }
    }

    ListItems.SingleValue {
        text: i18n.tr("Rotation")
        value: {
            console.warn('display orientation', model.orientation);
            switch (localOrientation || model.orientation) {
                case Display.Normal:
                    return i18n.tr("Normal");
                    break;
                case Display.Left:
                    return i18n.tr("Left");
                    break;
                case Display.Inverted:
                    return i18n.tr("Inverted");
                    break;
                case Display.Right:
                    return i18n.tr("Right");
                    break;
                default:
                    throw "Unable to determine orientation type.";
            }
        }
        visible: enabledCheck.checked
        progression: true
        onClicked: {
            var rotationPage = pageStack.push(
                Qt.resolvedUrl("PageRotation.qml"), {
                    display: model
                }
            );
            rotationPage.orientationChanged.connect(
                function (orientation) {
                    console.warn('locally setting orientation', orientation);
                    localOrientation = orientation;
                }
            );
        }
    }

    ListItems.SingleValue {
        text: i18n.tr("Resolution")
        value: localMode || model.mode
        visible: enabledCheck.checked
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
                    console.warn('locally setting mode', mode);
                    localMode = mode;
                }
            );
        }
    }

    // SettingsItemTitle {
    //     text: i18n.tr("Scale UI elements")
    //     visible: enabledCheck.checked
    //     showDivider: false
    // }

    // Menus.SliderMenu {
    //     id: scaleSlider
    //     objectName: "scaleSlider"
    //     visible: enabledCheck.checked
    //     live: true
    //     minimumValue: 0.0
    //     maximumValue: 100.0
    //     value: localScale >= 0 ? localScale : model.scale
    //     onUpdated: localScale = value
    // }

    // ListItems.Divider { opacity: 0 }

    Column {
        anchors {
            left: parent.left;
            right: parent.right
            leftMargin: spacing
            rightMargin: spacing
        }
        visible: enabledCheck.checked
        spacing: units.gu(1)

        Button {
            anchors { left: parent.left; right: parent.right }
            text: i18n.tr("Apply changes")
            enabled: localOrientation ||
                     localMode ||
                     localScale >= 0
            onClicked: {
                if (localOrientation) {
                    model.orientation = localOrientation;
                    localOrientation = null;
                }

                if (localMode) {
                    model.resolution = localMode;
                    localMode = "";
                }

                if (localScale >= 0) {
                    model.scale = localScale;
                    localScale = -1;
                }
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
                            "Failed to get system-update " +
                            "pageComponent"
                        );
                } else {
                    console.warn(
                        "Failed to get system-update plugin " +
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
