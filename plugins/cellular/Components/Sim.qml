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
    id: root
    property alias modem: modem
    property alias netReg: netReg
    property alias radioSettings: radioSettings
    property alias simMng: simMng
    property alias connMan: connMan

    property alias present: simMng.present
    property string path
    property string name
    property string title: {
        var number = simMng.subscriberNumbers[0] || simMng.subscriberIdentity;
        return name + (number ? " (" + number + ")" : "");
    }

    OfonoModem {
        id: modem
        modemPath: path
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
        Component.onCompleted: {
            console.warn('sim manager complete, path:', path)
            console.warn('sim manager complete, present:', simMng.present, simMng)
            console.warn('sim manager complete, SubscriberNumbers', simMng.subscriberNumbers, subscriberNumbers)
        }
        onPresenceChanged: {
            console.warn('sim manager onPresenceChanged, present changed:', present, simMng)
        }
        onSubscriberNumbersChanged: {
            console.warn('sim subscriberNumbers changed')
        }
    }

    OfonoConnMan {
        id: connMan
        modemPath: path
    }

    Binding {
        target: root
        property: "present"
        value: simMng.present
    }
}
