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
import QtQuick 2.4
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.SystemSettings.Brightness 1.0
import Ubuntu.Settings.Menus 0.1 as Menus
import Ubuntu.Settings.Components 0.1 as USC
import QMenuModel 0.1

ItemPage {
    id: root
    objectName: "brightnessPage"

    title: brightnessPanel.widiSupported ? i18n.tr("Brightness & Display") : i18n.tr("Brightness")
    flickable: scrollWidget

    /* We need to disable keyboard anchoring because we implement the
    KeyboardRectangle pattern. */
    Binding {
        target: main
        property: "anchorToKeyboard"
        value: false
    }

    function formatMode(mode) {
        mode = mode.split("x");
        /* TRANSLATORS: %1 refer to the amount of horizontal pixels in a
        display resolution, and %2 to the vertical pixels. E.g. 1200x720.
        %3 is the refresh rate in hz. */
        return i18n.tr("%1×%2 @ %3hz").arg(mode[0]).arg(mode[1]).arg(mode[2]);
    }

    GSettings {
        id: gsettings
        schema.id: "com.ubuntu.touch.system"
    }

    AethercastDisplays {
        id: aethercastDisplays
        objectName: "aethercastDisplays"
        onEnabledChanged: {
            /* This is a hack to ensure the aethercast enabled switch stays
             * in sync with the enabled property
             */
            enabledCheck.serverChecked = enabled;
            enabledCheck.checked = enabledCheck.serverChecked;
        }
    }

    DisplayModel {
        id: displayModel
        objectName: "displayModel"
        Component.onCompleted: console.log(displayModel.count)
    }

    UbuntuBrightnessPanel {
        id: brightnessPanel
        objectName: "brightnessPanel"
    }

    QDBusActionGroup {
        id: indicatorPower
        busType: 1
        busName: "com.canonical.indicator.power"
        objectPath: "/com/canonical/indicator/power"
        property variant brightness: action("brightness")
        Component.onCompleted: start()
    }

    Flickable {
        id: scrollWidget
        anchors {
            fill: parent
            bottomMargin: keyboardButtons.height + keyboard.height
        }
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
        /* Set the direction to workaround
           https://bugreports.qt-project.org/browse/QTBUG-31905 otherwise the UI
           might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            SettingsListItems.Standard {
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

            SettingsListItems.Standard {
                id: adjust
                text: i18n.tr("Adjust automatically")
                visible: brightnessPanel.powerdRunning &&
                         brightnessPanel.autoBrightnessAvailable

                CheckBox {
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
                visible: brightnessPanel.widiSupported
            }

            SettingsListItems.Standard {
                objectName: "externalDisplayControl"
                text: i18n.tr("External display")
                enabled: brightnessPanel.widiSupported
                onClicked: enabledCheck.trigger()

                Switch {
                    id: enabledCheck
                    property bool serverChecked: aethercastDisplays.enabled
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: {
                        aethercastDisplays.enabled = checked;
                    }
                }
            }

            SettingsListItems.SingleValueProgression {
                objectName: "displayCasting"
                visible: brightnessPanel.widiSupported
                enabled: aethercastDisplays.enabled
                text: i18n.tr("Wireless display")
                value: aethercastDisplays.state === "connected" ? i18n.tr("Connected") : i18n.tr("Not connected")
                onClicked: pageStack.push(Qt.resolvedUrl("WifiDisplays.qml"))
            }

            Repeater {
                model: displayModel

                Column {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    Label {
                        visible: displayModel.count > 1
                        anchors {
                            left: parent.left
                            right: parent.right
                            margins: units.gu(2)
                        }

                        text: displayName
                    }

                    SettingsItemTitle {
                        text: i18n.tr("Rotation:")
                    }

                    OptionSelector {
                        id: rotationSelector
                        objectName: "rotationSelector"
                        property bool _expanded: false
                        anchors {
                            left: parent.left
                            right: parent.right
                            margins: units.gu(2)
                        }
                        containerHeight: itemHeight * 4
                        model: [
                            i18n.tr("None"),
                            i18n.tr("90° clockwise"),
                            i18n.tr("180°"),
                            i18n.tr("270°")
                        ]
                        onDelegateClicked: expanded = !currentlyExpanded
                    }

                    SettingsItemTitle {
                        text: availableModes.length > 1 ?
                            i18n.tr("Resolution:") :
                            /* TRANSLATORS: %1 is a display resolution, e.g.
                            1200x720. */
                            i18n.tr("Resolution: %1").arg(formatMode(mode))
                    }

                    OptionSelector {
                        id: resolutionSelector
                        objectName: "resolutionSelector"
                        property bool _expanded: false
                        anchors {
                            left: parent.left
                            right: parent.right
                            margins: units.gu(2)
                        }
                        visible: availableModes.length > 1
                        containerHeight: itemHeight * availableModes.length
                        model: availableModes
                        onDelegateClicked: expanded = !currentlyExpanded
                        delegate: OptionSelectorDelegate {
                            text: formatMode(modelData)
                        }
                    }

                    SettingsItemTitle {
                        text: i18n.tr("Scale screen elements:")
                    }

                    Menus.SliderMenu {
                        id: scaleSlider
                        objectName: "scaleSlider"
                        minimumValue: 1.0
                        maximumValue: 100.0
                        value: scale
                        minIcon: "image://theme/grip-large"
                        maxIcon: "image://theme/view-grid-symbolic"
                    }
                }
            }
        }
    }

    Rectangle {
        id: keyboardButtons
        objectName: "keyboardButtons"
        anchors {
            left: parent.left
            right: parent.right
            bottom: keyboard.top
        }
        color: Theme.palette.selected.background

        // TODO: show always?
        visible: true
        height: units.gu(6)
        Button {
            id: applyButton
            objectName: "applyButton"
            anchors {
                left: parent.left
                leftMargin: units.gu(1)
                verticalCenter: parent.verticalCenter
            }
            // TODO: enabled when change was made
            enabled: true
            text: i18n.tr("Apply Changes…")
            onClicked: brightnessPanel.applyDisplayConfiguration()
        }
    }

    KeyboardRectangle {
        id: keyboard
        objectName: "keyboard"
        anchors.bottom: parent.bottom
    }
}
