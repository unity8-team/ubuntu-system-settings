/*
 * Copyright (C) 2014 Canonical Ltd
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
import MeeGo.QOfono 0.2

Item {
    property alias modem: modem
    property alias radioSettings: radioSettings
    property alias simMng: simMng
    property alias connMan: connMan

    property string path
    property string name: getName()
    property string title: {
        var number = simMng.subscriberNumbers[0] || simMng.subscriberIdentity;
        return name + (number ? " (" + number + ")" : "");
    }
    property var settings

    OfonoModem {
        id: modem
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

    function getName () {
        return (path === settings.sim1Path) ? settings.sim1Name : settings.sim2Name;
    }

    Connections {
        target: settings
        onChanged: {
            console.warn('settings changed');
        }
    }

}
