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
import QMenuModel 0.1 as QMenuModel
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Wifi 1.0
import Ubuntu.SystemSettings.Wizard.Utils 0.1
import Ubuntu.Settings.Menus 0.1 as Menus
import "../Components" as LocalComponents

LocalComponents.Page {
    id: wifiPage
    title: i18n.tr("Connect to Wi-Fi")
    forwardButtonSourceComponent: forwardButton

    readonly property bool connected: mainMenu.connectedAPs === 1

    function getExtendedProperty(object, propertyName, defaultValue) {
        if (object && object.hasOwnProperty(propertyName)) {
            return object[propertyName];
        }
        return defaultValue;
    }

    QMenuModel.UnityMenuModel {
        id: menuModel
        busName: "com.canonical.indicator.network"
        actions: { "indicator": "/com/canonical/indicator/network" }
        menuObjectPath: "/com/canonical/indicator/network/phone_wifi_settings"
    }

    Component {
        id: hiddenComponent
        Item {
            height: 0
            visible: false
        }
    }

    Component {
        id: accessPointComponent
        ListItem.Standard {
            id: accessPoint
            objectName: "accessPoint"

            property QtObject menuData: null
            property var unityMenuModel: menuModel
            property var extendedData: menuData && menuData.ext || undefined
            property var strengthAction: QMenuModel.UnityMenuAction {
                model: unityMenuModel
                index: menuIndex
                name: getExtendedProperty(extendedData, "xCanonicalWifiApStrengthAction", "")
            }
            property bool checked: menuData && menuData.isToggled || false
            property bool secure: getExtendedProperty(extendedData, "xCanonicalWifiApIsSecure", false)
            property bool adHoc: getExtendedProperty(extendedData, "xCanonicalWifiApIsAdhoc", false)
            property int signalStrength: strengthAction.valid ? strengthAction.state : 0
            property int menuIndex: -1

            function loadAttributes() {
                if (!unityMenuModel || menuIndex == -1) return;
                unityMenuModel.loadExtendedAttributes(menuIndex, {'x-canonical-wifi-ap-is-adhoc': 'bool',
                                                                  'x-canonical-wifi-ap-is-secure': 'bool',
                                                                  'x-canonical-wifi-ap-strength-action': 'string'});
            }

            signal activate()

            text: menuData && menuData.label || ""
            enabled: menuData && menuData.sensitive || false
            iconName: {
                var imageName = "nm-signal-100";

                if (adHoc) {
                    imageName = "nm-adhoc";
                } else if (signalStrength == 0) {
                    imageName = "nm-signal-00";
                } else if (signalStrength <= 25) {
                    imageName = "nm-signal-25";
                } else if (signalStrength <= 50) {
                    imageName = "nm-signal-50";
                } else if (signalStrength <= 75) {
                    imageName = "nm-signal-75";
                }

                if (secure) {
                    imageName += "-secure";
                }
                return imageName;
            }
            iconFrame: false
            control: CheckBox {
                id: checkBoxActive

                onClicked: {
                    accessPoint.activate();
                }
            }
            style: Rectangle {
                color: "#4c000000"
            }

            Component.onCompleted: {
                loadAttributes();
            }
            onUnityMenuModelChanged: {
                loadAttributes();
            }
            onMenuIndexChanged: {
                loadAttributes();
            }
            onCheckedChanged: {
                // Can't rely on binding. Checked is assigned on click.
                checkBoxActive.checked = checked;
                if (checked) {
                    mainMenu.connectedAPs++
                } else {
                    mainMenu.connectedAPs--
                }
            }
            onActivate: unityMenuModel.activate(menuIndex);
        }
    }

    Column {
        id: column
        spacing: units.gu(2)
        anchors {
            top: content.top
            bottom: content.bottom
            left: wifiPage.left
            right: wifiPage.right
        }

        Label {
            id: label
            anchors {
                left: parent.left
                leftMargin: leftMargin
                right: parent.right
                rightMargin: rightMargin
            }
            fontSize: "small"
            text: i18n.tr("Available networks")
        }

        ListView {
            id: mainMenu

            // FIXME Check connection betterhttps://bugs.launchpad.net/indicator-network/+bug/1349371
            property int connectedAPs: 0

            anchors { left: parent.left; right: parent.right; }
            height: column.height - label.height - column.spacing
            clip: true
            flickDeceleration: 1500 * units.gridUnit / 8
            maximumFlickVelocity: 2500 * units.gridUnit / 8
            model: menuModel
            cacheBuffer: mainMenu.height * 2
            boundsBehavior: (contentHeight > mainMenu.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

            delegate: Loader {
                id: loader
                anchors { left: parent.left; right: parent.right; }
                asynchronous: true
                sourceComponent: model.type === "unity.widgets.systemsettings.tablet.accesspoint" ? accessPointComponent : hiddenComponent

                onLoaded: {
                    if (sourceComponent === accessPointComponent) {
                        item.menuData = Qt.binding(function() { return model; });
                        item.menuIndex = Qt.binding(function() { return index; });
                    }
                }
            }
        }
    }

    Component {
        id: forwardButton
        LocalComponents.StackButton {
            text: connected ? i18n.tr("Continue") : i18n.tr("Skip")
            rightArrow: !connected
            onClicked: pageStack.next()
        }
    }
}
