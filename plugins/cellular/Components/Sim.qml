/*
 * Copyright (C) 2014-2016 Canonical Ltd
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
import QtQuick 2.4
import MeeGo.QOfono 0.2

Item {
    id: root
    property alias modem: modem
    property alias netReg: netReg
    property alias radioSettings: radioSettings
    property alias simMng: simMng
    property alias connMan: connMan
    property alias mtkSettings: mtkSettings

    property alias present: simMng.present
    property string path
    property string name
    property string title: {
        var number = simMng.subscriberNumbers[0] || simMng.subscriberIdentity;
        return name + (number ? " (" + number + ")" : "");
    }

    function getTechString () {
        return techToString(radioSettings.technologyPreference);
    }

    function techToString (tech) {
        var strings = {
            'gsm': i18n.tr("2G only (saves battery)"),
            'umts': i18n.tr("2G/3G (faster)"),
            'lte': i18n.tr("2G/3G/4G (faster)")
        };
        strings['umts_enable'] = strings['umts'];
        return strings[tech] || "";
    }

    // adds umts_enable to an copy of model
    function addUmtsEnableToModel (model) {
        var newModel = model.slice(0);
        newModel.push('umts_enable');
        return newModel;
    }

    OfonoModem {
        id: modem
        modemPath: path
        onInterfacesChanged: {
            if (interfaces.indexOf('org.ofono.MtkSettings') >= 0) {
                mtkSettings._valid = true;
            } else {
                mtkSettings._valid = false;
            }
        }
    }

    OfonoNetworkRegistration {
        id: netReg
        modemPath: path
    }

    OfonoRadioSettings {
        id: radioSettings
        modemPath: path
    }

    OfonoSimManager {
        id: simMng
        modemPath: path
    }

    OfonoConnMan {
        id: connMan
        modemPath: path
    }

    OfonoMtkSettings {
        id: mtkSettings
        property bool _valid: true
        modemPath: path
        on_ValidChanged: {
            if (_valid) {
                // invalidate the binding's dbus proxy
                modemPath = "/invalid";
                modemPath = root.path;
            }
        }
    }
}
