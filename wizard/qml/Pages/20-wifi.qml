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

    SortFilterProxyModel {
        id: menuModel
        filterRole: 5 // UnityMenuModel.TypeRole should really be exported from unitymenumodel
        filterRegExp: RegExp("unity.widgets.systemsettings.tablet.accesspoint")
        model: QMenuModel.UnityMenuModel {
            id: unitymenumodel
            busName: "com.canonical.indicator.network"
            actions: { "indicator": "/com/canonical/indicator/network" }
            menuObjectPath: "/com/canonical/indicator/network/phone_wifi_settings"
        }
    }

    Column {
        id: column
        spacing: units.gu(2)
        anchors.top: content.top
        anchors.bottom: content.bottom
        anchors.left: wifiPage.left
        anchors.right: wifiPage.right

        Label {
            id: label
            anchors.left: parent.left
            anchors.leftMargin: leftMargin
            anchors.right: parent.right
            anchors.rightMargin: rightMargin
            fontSize: "small"
            text: i18n.tr("Available networks")
        }

        ListView {
            id: mainMenu

            property int connectedAPs: 0

            model: menuModel
            height: column.height - label.height - column.spacing
            anchors.left: parent.left
            anchors.right: parent.right
            clip: true

            // Ensure all delegates are cached in order to improve smoothness of scrolling
            cacheBuffer: 10000

            // Only allow flicking if the content doesn't fit on the page
            boundsBehavior: (contentHeight > mainMenu.height) ?
                                Flickable.DragAndOvershootBounds :
                                Flickable.StopAtBounds

            currentIndex: -1
            delegate: Menus.AccessPointMenu {
                id: menuDelegate

                property int menuIndex: menuModel.mapRowToSource(index)
                property var extendedData: model.ext
                property var serverToggle: model.isToggled
                property var strengthAction: QMenuModel.UnityMenuAction {
                    model: unitymenumodel
                    index: menuIndex
                    name: getExtendedProperty(extendedData, "xCanonicalWifiApStrengthAction", "")
                }

                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: model.label
                enabled: model.sensitive
                secure: getExtendedProperty(extendedData, "xCanonicalWifiApIsSecure", false)
                adHoc: getExtendedProperty(extendedData, "xCanonicalWifiApIsAdhoc", false)
                signalStrength: strengthAction.valid ? strengthAction.state : 0

                style: Rectangle {
                    color: "black"
                    opacity: 0.3
                }

                onCheckedChanged: {
                    if (checked) {
                        mainMenu.connectedAPs++
                    } else {
                        mainMenu.connectedAPs--
                    }
                }
                onMenuIndexChanged: {
                    loadAttributes();
                }
                onServerToggleChanged: {
                    checked = serverToggle;
                }
                onTriggered: {
                    unitymenumodel.activate(menuIndex);
                }

                function loadAttributes() {
                    if (!unitymenumodel || menuIndex == -1) return;
                    unitymenumodel.loadExtendedAttributes(menuIndex, {'x-canonical-wifi-ap-is-adhoc': 'bool',
                                                                      'x-canonical-wifi-ap-is-secure': 'bool',
                                                                      'x-canonical-wifi-ap-strength-action': 'string'});
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
