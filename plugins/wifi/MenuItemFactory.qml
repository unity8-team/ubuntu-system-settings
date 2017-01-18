/*
 * Copyright 2013-2016 Canonical Ltd.
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
 *      Nick Dedekind <nick.dedekind@canonical.com>
  */

import QtQuick 2.4
import QMenuModel 0.1 as QMenuModel
import Ubuntu.Settings.Menus 0.1 as Menus
import Ubuntu.Settings.Components 0.1 as USC

Item {
    id: menuFactory

    property var model: null

    property var _map:  {
        "unity.widgets.systemsettings.tablet.switch"        : switchMenu,

        "com.canonical.indicator.div"       : divMenu,
        "com.canonical.indicator.section"   : sectionMenu,
        "com.canonical.indicator.switch"    : switchMenu,

        "com.canonical.unity.switch"    : switchMenu,

        "unity.widgets.systemsettings.tablet.wifisection" : wifiSection,
        "unity.widgets.systemsettings.tablet.accesspoint" : accessPoint,
    }

    function getExtendedProperty(object, propertyName, defaultValue) {
        if (object && object.hasOwnProperty(propertyName)) {
            return object[propertyName];
        }
        return defaultValue;
    }

    Component { id: divMenu; DivMenuItem {} }

    Component {
        id: sectionMenu;
        SectionMenuItem {
            property QtObject menu: null

            text: menu && menu.label ? menu.label : ""
        }
    }

    Component {
        id: standardMenu;
        StandardMenuItem {
            property QtObject menu: null
            property int menuIndex: -1

            text: menu && menu.label ? menu.label : ""
            iconName: menu ? menu.icon : ""
            checkable: menu ? (menu.isCheck || menu.isRadio) : false
            checked: checkable ? menu.isToggled : false
            enabled: menu ? menu.sensitive : false

            onActivate: model.activate(menuIndex);
        }
    }

    Component {
        id: switchMenu;
        Menus.SwitchMenu {
            id: switchItem
            property QtObject menu: null
            property int menuIndex: -1
            property bool serverChecked: menu && menu.isToggled || false

            text: menu && menu.label || ""
            iconSource: menu && menu.icon || ""
            checked: serverChecked
            enabled: menu && menu.sensitive || false

            USC.ServerPropertySynchroniser {
                userTarget: switchItem
                userProperty: "checked"
                serverTarget: switchItem
                serverProperty: "serverChecked"

                onSyncTriggered: model.activate(switchItem.menuIndex)
            }
        }
    }

    Component {
        id: wifiSection;
        SectionMenuItem {
            property QtObject menu: null
            property var menuModel: menuFactory.menuModel
            property int menuIndex: -1
            property var extendedData: menu && menu.ext || undefined
            text: menu && menu.label ? menu.label : ""
            busy: getExtendedProperty(extendedData, "xCanonicalBusyAction", false)

            onMenuModelChanged: {
                loadAttributes();
            }
            onMenuIndexChanged: {
                loadAttributes();
            }

            function loadAttributes() {
                if (!menuModel || menuIndex == undefined) return;
                menuModel.loadExtendedAttributes(menuIndex, {'x-canonical-busy-action': 'bool'})
            }
        }
    }

    Component {
        id: accessPoint;
        AccessPoint {
            id: apItem
            property QtObject menu: null
            property var menuModel: menuFactory.model
            property int menuIndex: -1
            property var extendedData: menu && menu.ext || undefined
            property var strengthAction: QMenuModel.UnityMenuAction {
                model: menuModel ? menuModel : null
                name: getExtendedProperty(extendedData, "xCanonicalWifiApStrengthAction", "")
            }
            property bool serverChecked: menu && menu.isToggled || false

            text: menu && menu.label ? menu.label : ""
            secure: getExtendedProperty(extendedData, "xCanonicalWifiApIsSecure", false)
            adHoc: getExtendedProperty(extendedData, "xCanonicalWifiApIsAdhoc", false)
            checked: serverChecked
            signalStrength: {
                if (strengthAction.valid) {
                    var state = strengthAction.state; // handle both int and uchar
                    if (typeof state == "string") {
                        return state.charCodeAt();
                    }
                    return state;
                }
                return 0;
            }
            enabled: menu ? menu.sensitive : false

            onMenuModelChanged: {
                loadAttributes();
            }
            onMenuIndexChanged: {
                loadAttributes();
            }

            resources: USC.ServerPropertySynchroniser {
                userTarget: apItem
                userProperty: "active"
                userTrigger: "onActivate"
                serverTarget: apItem
                serverProperty: "serverChecked"

                onSyncTriggered: model.activate(apItem.menuIndex)
            }

            function loadAttributes() {
                if (!model || menuIndex == undefined) return;
                model.loadExtendedAttributes(menuIndex, {'x-canonical-wifi-ap-is-adhoc': 'bool',
                                                         'x-canonical-wifi-ap-is-secure': 'bool',
                                                         'x-canonical-wifi-ap-strength-action': 'string'});
            }
        }
    }

    function load(modelData) {
        if (modelData.type !== undefined) {
            var component = _map[modelData.type];
            if (component !== undefined) {
                return component;
            }
        } else {
            if (modelData.isSeparator) {
                return divMenu;
            }
        }
        return standardMenu;
    }
}
