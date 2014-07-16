/*
 * Copyright 2014 Canonical Ltd.
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
 *
 * Authors:
 *      Ken VanDine <ken.vandine@canonical.com>
 */

import QtQuick 2.0
import QMenuModel 0.1 as QMenuModel
import Ubuntu.Settings.Menus 0.1 as Menus

Item {
    id: menuFactory

    property var menuModel: null

    property var _map:  {
        "unity.widgets.systemsettings.tablet.volumecontrol" : sliderMenu,
        "com.canonical.indicator.slider"         : sliderMenu,
        "com.canonical.unity.slider"    : sliderMenu,
        "com.canonical.indicator.root"    : menuitem,
    }

    function getExtendedProperty(object, propertyName, defaultValue) {
        console.warn("getExtendedProperty propertyName:" + propertyName);
        if (object && object.hasOwnProperty(propertyName)) {
            return object[propertyName];
        }
        return defaultValue;
    }

    Component {
        id: sliderMenu;

        Menus.SliderMenu {
            objectName: "sliderMenu"
            property QtObject menuData: null
            property var menuModel: menuFactory.menuModel
            property int menuIndex: -1
            property var extendedData: menuData && menuData.ext || undefined
            property var serverValue: getExtendedProperty(menuData, "actionState", undefined)

            text: menuData && menuData.label || ""
            iconSource: menuData && menuData.icon || ""
            minIcon: getExtendedProperty(extendedData, "minIcon", "")
            maxIcon: getExtendedProperty(extendedData, "maxIcon", "")

            minimumValue: getExtendedProperty(extendedData, "minValue", 0.0)
            maximumValue: {
                var maximum = getExtendedProperty(extendedData, "maxValue", 1.0);
                if (maximum <= minimumValue) {
                        return minimumValue + 1;
                }
                return maximum;
            }
            enabled: true//menuData && menuData.sensitive || false

            onMenuDataChanged: {
                console.warn("onMenuDataChanged extendedData: " + menuData.ext);
                loadAttributes();
                extendedData = menuData.ext;
                console.warn("onMenuDataChanged minIcon: " + minIcon);
            }

            onMenuModelChanged: {
                loadAttributes();
                console.warn("onMenuModelChanged: minIcon " + minIcon + " maxIcon " + maxIcon);
            }
            onMenuIndexChanged: {
                loadAttributes();
                console.warn("onMenuIndexChanged: menuIndex " + menuIndex);
            }
            onServerValueChanged: {
                // value can be changed by slider, so a binding won't work.
                console.warn("onServerValueChanged: serverValue " + serverValue);
                console.warn("onServerValueChanged: value " + value);
                if (serverValue !== undefined) {
                    value = serverValue;
                }
            }

            onUpdated: {
                console.warn("onUpdated: index " + menuIndex + " value " + value);
                menuModel.changeState(menuIndex, value);
            }

            function loadAttributes() {
                console.warn("loadAttributes: " + menuIndex);
                if (!menuModel || menuIndex == -1) return;
                menuModel.loadExtendedAttributes(menuIndex, {'min-value': 'double',
                                                             'max-value': 'double',
                                                             'min-icon': 'icon',
                                                             'max-icon': 'icon'});
            }
        }
    }

            Component {
                id: menuitem
                Rectangle {
                    width: parent.width
                    height: 40
                    color: "#ddd"
                    property QtObject menuData: null
                    Row {
                        anchors.fill: parent
                        anchors.margins: 5
                        Image {
                            source: menuData.icon
                        }
                        Text {
                            height: parent.height
                            verticalAlignment: Text.AlignVCenter
                            color: menuData.sensitive ? "black" : "#aaa";
                            text: menuData.label
                        }
                    }
                }
            }


    function load(modelData) {
        console.warn("load: " + modelData.type);
        if (modelData.type !== undefined) {
            var component = _map[modelData.type];
            if (component !== undefined) {
                return component;
            }
        //} else {
        //    if (modelData.isSeparator) {
        //        return divMenu;
        //    }
        }
        //return menuitem;
    }
}
