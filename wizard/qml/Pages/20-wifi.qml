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
import Ubuntu.SystemSettings.Utils 0.1
import Ubuntu.Settings.Menus 0.1 as Menus
import "../Components" as LocalComponents

LocalComponents.Page {
    id: wifiPage
    title: i18n.tr("Connect to Wi-Fi")
    forwardButtonSourceComponent: forwardButton

    property string busName: "com.canonical.indicator.network"
    property string actionsObjectPath: "/com/canonical/indicator/network"
    property var menuObjectPaths: {"phone_wifi_settings": "/com/canonical/indicator/network/phone_wifi_settings"}
    readonly property string device: "phone_wifi_settings"
    property string rootMenuType: "com.canonical.indicator.root"
    property bool active: true
    property string deviceMenuObjectPath: menuObjectPaths.hasOwnProperty(device) ? menuObjectPaths[device] : ""

    signal actionGroupUpdated()
    signal modelUpdated()

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

    Item {
        id: content
        anchors {
            fill: parent
            topMargin: units.gu(1)
            bottomMargin: bottomMargin
        }

        Label {
            id: topLabel
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                leftMargin: leftMargin
                rightMargin: rightMargin
            }
            wrapMode: Text.WordWrap
            fontSize: "large"
            text: i18n.tr("Available networks")
        }

        Menus.SeparatorMenu {
            id: separator
            anchors {
                top: topLabel.bottom
                left: parent.left
                right: parent.right
                topMargin: units.gu(1)
            }
            height: units.dp(2)
        }

        Label {
            id: label
            anchors {
                top: separator.bottom
                left: parent.left
                right: parent.right
                topMargin: units.gu(1)
                leftMargin: leftMargin
                rightMargin: rightMargin
            }
            wrapMode: Text.WordWrap
            text: i18n.tr("Connect to a Wi-Fi network to customize your setup.")
        }

        UbuntuShape {
            anchors {
                top: label.bottom
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                topMargin: units.gu(2)
                leftMargin: leftMargin
                rightMargin: rightMargin
            }
            radius: "medium"

            ListView {
                id: mainMenu
                model: menuModel
                anchors.fill: parent
                clip: true

                // Ensure all delegates are cached in order to improve smoothness of scrolling
                cacheBuffer: 10000

                // Only allow flicking if the content doesn't fit on the page
                contentHeight: contentItem.childrenRect.height
                boundsBehavior: (contentHeight > mainMenu.height) ?
                                    Flickable.DragAndOvershootBounds :
                                    Flickable.StopAtBounds

                currentIndex: -1
                delegate: Item {
                    id: menuDelegate

                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: loader.height
                    visible: height > 0

                    Loader {
                        id: loader
                        asynchronous: true

                        property int modelIndex: index

                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        sourceComponent: accessPoint

                        onLoaded: {
                            if (item.hasOwnProperty("menuData")) {
                                item.menuData = Qt.binding(function() { return model; });
                            }
                            if (item.hasOwnProperty("menuIndex")) {
                                item.menuIndex = Qt.binding(function() { return modelIndex; });
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: accessPoint;
        Menus.AccessPointMenu {
            objectName: "accessPoint"
            property QtObject menuData: null
            property var menuModel: unitymenumodel
            property int menuIndex: -1
            property var extendedData: menuData && menuData.ext || undefined

            property var strengthAction: QMenuModel.UnityMenuAction {
                model: menuModel
                index: menuIndex
                name: getExtendedProperty(extendedData, "xCanonicalWifiApStrengthAction", "")
            }

            text: menuData && menuData.label || ""
            enabled: menuData && menuData.sensitive || false
            checked: menuData && menuData.isToggled || false
            secure: getExtendedProperty(extendedData, "xCanonicalWifiApIsSecure", false)
            adHoc: getExtendedProperty(extendedData, "xCanonicalWifiApIsAdhoc", false)
            signalStrength: strengthAction.valid ? strengthAction.state : 0

            onMenuModelChanged: {
                loadAttributes();
            }
            onMenuIndexChanged: {
                loadAttributes();
            }
            onTriggered: {
                menuModel.activate(menuIndex);
                console.log(menuIndex)
            }

            function loadAttributes() {
                if (!menuModel || menuIndex == -1) return;
                menuModel.loadExtendedAttributes(menuIndex, {'x-canonical-wifi-ap-is-adhoc': 'bool',
                                                             'x-canonical-wifi-ap-is-secure': 'bool',
                                                             'x-canonical-wifi-ap-strength-action': 'string'});
            }
        }
    }

    Component {
        id: forwardButton
        LocalComponents.ForwardButton {
            text: i18n.tr("Connect")
            onClicked: pageStack.next()
        }
    }
}
