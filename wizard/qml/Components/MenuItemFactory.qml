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
 *      Nick Dedekind <nick.dedekind@canonical.com>
 */

import QtQuick 2.0
import QMenuModel 0.1 as QMenuModel

Item {
    id: menuFactory

    property var model: null

    property var _map:  {
        "unity.widgets.systemsettings.tablet.accesspoint" : accessPoint,
    }

    Component {
        id: accessPoint;
        AccessPoint {
            property QtObject menu: null
            property var strenthAction: QMenuModel.UnityMenuAction {
                model: menuFactory.model ? menuFactory.model : null
                name: menu ? menu.ext.xCanonicalWifiApStrengthAction : ""
            }

            text: menu && menu.label ? menu.label : ""
            icon: menu ? menu.icon : ""
            secure: menu ? menu.ext.xCanonicalWifiApIsSecure : false
            adHoc: menu ? menu.ext.xCanonicalWifiApIsAdhoc : false
            checked: menu ? menu.isToggled : false
            signalStrength: strenthAction.valid ? strenthAction.state : 0
            enabled: menu ? menu.sensitive : false

            Component.onCompleted: {
                model.loadExtendedAttributes(modelIndex, {'x-canonical-wifi-ap-is-adhoc': 'bool',
                                                          'x-canonical-wifi-ap-is-secure': 'bool',
                                                          'x-canonical-wifi-ap-strength-action': 'string'});
            }
            onActivate: model.activate(modelIndex);
        }
    }

    function load(modelData) {
        if (modelData.type !== undefined) {
            var component = _map[modelData.type];
            if (component !== undefined) {
                return component;
            }
        }
        /*else {
            if (modelData.isSeparator) {
                return divMenu;
            }
        }
        return standardMenu; */
    }
}
