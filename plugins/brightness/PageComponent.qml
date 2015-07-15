/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-14 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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
 */

import GSettings 1.0
import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Brightness 1.0
import Ubuntu.Settings.Menus 0.1 as Menus
import Ubuntu.Settings.Components 0.1 as USC
import QMenuModel 0.1
import "displays.js" as Manager

ItemPage {
    id: root
    objectName: "brightnessPage"

    title: i18n.tr("Brightness")

    UbuntuBrightnessPanel {
        id: brightnessPanel
    }

    Displays {
        id: displays
    }

    Component {
        id: displayComponent
        Display {}
    }

    ListModel {
        id: displaysModel
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        QDBusActionGroup {
            id: indicatorPower
            busType: 1
            busName: "com.canonical.indicator.power"
            objectPath: "/com/canonical/indicator/power"

            property variant brightness: action("brightness")

            Component.onCompleted: start()
        }

        SettingsItemTitle {
            text: i18n.tr("Display brightness")
            showDivider: false
        }

        /* Use the SliderMenu component instead of the Slider to avoid binding
           issues on valueChanged until LP: #1388094 is fixed.
        */
        Menus.SliderMenu {
            id: brightnessSlider
            objectName: "sliderMenu"
            enabled: indicatorPower.brightness.state != null
            live: true
            minimumValue: 0.0
            maximumValue: 100.0
            minIcon: "image://theme/display-brightness-min"
            maxIcon: "image://theme/display-brightness-max"

            property real serverValue: enabled ? indicatorPower.brightness.state * 100 : 0.0

            USC.ServerPropertySynchroniser {
                userTarget: brightnessSlider
                userProperty: "value"
                serverTarget: brightnessSlider
                serverProperty: "serverValue"
                maximumWaitBufferInterval: 16

                onSyncTriggered: indicatorPower.brightness.updateState(value / 100.0)
            }
        }

        ListItem.Standard {
            id: adjust
            text: i18n.tr("Adjust automatically")
            visible: brightnessPanel.powerdRunning &&
                     brightnessPanel.autoBrightnessAvailable
            control: CheckBox {
                id: autoAdjustCheck
                property bool serverChecked: gsettings.autoBrightness
                onServerCheckedChanged: checked = serverChecked
                Component.onCompleted: checked = serverChecked
                onTriggered: gsettings.autoBrightness = checked
            }
            showDivider: false
        }

        ListItem.Caption {
            text: i18n.tr(
                    "Brightens and dims the display to suit the surroundings.")
            visible: adjust.visible
        }

        ListItem.Divider {
            visible: displaysModel.count > 0
        }

        Repeater {
            model: displaysModel

            Column {
                anchors { left: parent.left; right: parent.right }

                property var localOrientation: null
                property string localResolution: ""
                property double localScale: -1

                SettingsItemTitle {
                    text: path
                    visible: model.count > 1
                }

                ListItem.Standard {
                    text: i18n.tr("External display")
                    enabled: display.connected
                    onClicked: enabledCheck.trigger()
                    control: CheckBox {
                        id: enabledCheck
                        property bool serverChecked: display.enabled
                        onServerCheckedChanged: checked = serverChecked
                        Component.onCompleted: checked = serverChecked
                        onTriggered: display.enabled = checked
                    }
                }

                ListItem.SingleValue {
                    text: i18n.tr("Rotation")
                    value: {
                        console.warn('display.orientation', display.orientation);
                        switch (localOrientation || display.orientation) {
                            case Display.AnyMode:
                                return i18n.tr("None");
                                break;
                            case Display.PortraitMode:
                            case Display.PortraitAnyMode:
                                return i18n.tr("90° clockwise");
                                break;
                            case Display.LandscapeMode:
                            case Display.LandscapeInvertedMode:
                            case Display.LandscapeAnyMode:
                                return i18n.tr("180° clockwise");
                                break;
                            case Display.PortraitInvertedMode:
                                return i18n.tr("270° clockwise");
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
                                display: display
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

                ListItem.SingleValue {
                    text: i18n.tr("Resolution")
                    value: localResolution || display.resolution
                    visible: enabledCheck.checked
                    progression: true
                    onClicked: {
                        var resPage = pageStack.push(
                            Qt.resolvedUrl("PageResolution.qml"), {
                                display: display
                            }
                        );
                        resPage.resolutionChanged.connect(
                            function (resolution) {
                                console.warn('locally setting resolution', resolution);
                                localResolution = resolution;
                            }
                        );
                    }
                }

                SettingsItemTitle {
                    text: i18n.tr("Scale UI elements")
                    visible: enabledCheck.checked
                    showDivider: false
                }

                /* Use the SliderMenu component instead of the Slider to avoid binding
                   issues on valueChanged until LP: #1388094 is fixed.
                */
                Menus.SliderMenu {
                    id: scaleSlider
                    objectName: "scaleSlider"
                    visible: enabledCheck.checked
                    live: true
                    minimumValue: 0.0
                    maximumValue: 100.0
                    value: localScale >= 0 ? localScale : display.scale
                    onUpdated: localScale = value
                }

                Column {
                    anchors {
                        left: parent.left;
                        right: parent.right
                        leftMargin: spacing
                        rightMargin: spacing
                    }
                    visible: enabledCheck.checked
                    spacing: units.gu(2)

                    ListItem.Divider { opacity: 0 }

                    Button {
                        anchors { left: parent.left; right: parent.right }
                        text: i18n.tr("Apply changes")
                        enabled: localOrientation ||
                                 localResolution ||
                                 localScale >= 0
                        onClicked: {
                            if (localOrientation) {
                                display.orientation = localOrientation;
                                localOrientation = null;
                            }

                            if (localResolution) {
                                display.resolution = localResolution;
                                localResolution = "";
                            }

                            if (localScale >= 0) {
                                display.scale = localScale;
                                localScale = -1;
                            }
                        }
                    }

                    ListItem.Divider { opacity: 0 }

                    Button {
                        anchors { left: parent.left; right: parent.right }
                        text: i18n.tr("Sound settings…")
                    }

                    Button {
                        anchors { left: parent.left; right: parent.right }
                        text: i18n.tr("External keyboard…")
                    }
                }
            }
        }
    }

    GSettings {
        id: gsettings
        schema.id: "com.ubuntu.touch.system"
    }

    Connections {
        target: displays

        onDisplaysChanged: Manager.displaysChanged(displays)
        Component.onCompleted: Manager.displaysChanged(displays.displays)
    }
}
