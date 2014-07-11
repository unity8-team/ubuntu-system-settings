/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/
import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import MeeGo.QOfono 0.2
import QMenuModel 0.1

Item {

    property var modemPath
    property var name
    //property bool preferred: netReg.modemPath === modemPath

    OfonoModem {
        id: modem
        modemPath: modemPath
    }

    OfonoRadioSettings {
        id: rdoSet
        modemPath: modemPath
        //onTechnologyPreferenceChanged: RSHelpers.preferenceChanged(preference);
    }

    OfonoSimManager {
        id: simMng
        modemPath: modemPath
    }

    OfonoConnMan {
        id: connMan
        modemPath: modemPath
        powered: techPrefSelector.selectedIndex !== 0
        //onPoweredChanged: RSHelpers.poweredChanged(powered);
    }

    Component.onCompleted: console.warn('OfonoGroup: created group for', modemPath)

}
