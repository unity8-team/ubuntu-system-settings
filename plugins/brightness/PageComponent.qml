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
import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Brightness 1.0
import GSettings 1.0
import QMenuModel 0.1

ItemPage {
    id: root
    objectName: "brightnessPage"

    title: i18n.tr("Brightness")

    UbuntuBrightnessPanel {
        id: brightnessPanel
    }

    QDBusActionGroup {
        id: indicatorPower
        busType: 1
        busName: "com.canonical.indicator.power"
        objectPath: "/com/canonical/indicator/power"

        property variant brightness: action("brightness")
    }

    GSettings {
        id: gsettings
        schema.id: "com.ubuntu.touch.system"
    }

    Component.onCompleted: indicatorPower.start()

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.Standard {
            text: i18n.tr("Display brightness")
            showDivider: false
        }

        ListItem.Base {

            Icon {
                id: iconLeft
                anchors.verticalCenter: parent.verticalCenter
                height: sliderId.height - units.gu(1)
                name: "torch-off"
                width: height

                MouseArea {
                    anchors.fill: parent
                    onClicked: sliderId.value = 0.0
                }
            }
            Slider {
                id: sliderId
                function formatValue(v) {
                    return "%1%".arg(v.toFixed(0))
                }

                anchors {
                    left: iconLeft.right
                    right: iconRight.left
                    leftMargin: units.gu(1)
                    rightMargin: units.gu(1)
                    verticalCenter: parent.verticalCenter
                }
                height: parent.height - units.gu(2)
                minimumValue: 0.0
                maximumValue: 100.0
                enabled: indicatorPower.brightness.state != null
                value: enabled ? indicatorPower.brightness.state * 100 : 0.0
                live: true

                onValueChanged: indicatorPower.brightness.updateState(
                                    value / 100.0)
            }
            Icon {
                id: iconRight
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                height: sliderId.height - units.gu(1)
                name: "torch-on"
                width: height

                MouseArea {
                    anchors.fill: parent
                    onClicked: sliderId.value = 100.0
                }
            }
        }

        Binding {
            target: adjust
            property: "checked"
            value: adjust.enabled && gsettings.autoBrightness
        }

        ListItem.Standard {
            id: adjust
            text: i18n.tr("Adjust automatically")
            enabled: brightnessPanel.powerdRunning &&
                     brightnessPanel.autoBrightnessAvailable
            control: CheckBox {
                id: autoAdjustSwitch
                onClicked: gsettings.autoBrightness = checked
            }
            Component.onCompleted: clicked.connect(autoAdjustSwitch.clicked)
            showDivider: false
        }
        ListItem.Caption {
            text: i18n.tr("Brightens and dims the display to suit the " +
                          "surroundings")
        }
    }
}
