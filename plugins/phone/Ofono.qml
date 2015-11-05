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
import QtQuick 2.4
import GSettings 1.0
import MeeGo.QOfono 0.2

Item {
    property alias netReg: netReg
    property alias simMng: simMng
    property alias present: simMng.present

    property string path
    property string name
    property string title: {
        var number = simMng.subscriberNumbers[0] || simMng.subscriberIdentity;
        return name + (number ? " (" + number + ")" : "");
    }

    OfonoNetworkRegistration {
        id: netReg
        modemPath: path
    }

    OfonoSimManager {
        id: simMng
        modemPath: path
    }

    function setCallForwardingSummary (val) {
        var tmp = {};
        var fwdSum = settings.callforwardingSummaries;
        for (var k in fwdSum){
            if (fwdSum.hasOwnProperty(k)) {
                tmp[k] = fwdSum[k];
            }
        }
        tmp[path] = val;
        tmp[path + '_updated_at'] = (new Date).getTime() / 1000;
        settings.callforwardingSummaries = tmp;
    }

    function getCallForwardingSummary () {
        var fwdSum = settings.callforwardingSummaries;
        var updated_at = fwdSum[path + '_updated_at'] || 0;

        // Returns an empty string one hour after the value was set.
        if (((new Date).getTime() / 1000) - updated_at > 3600) {
            return '';
        } else {
            return fwdSum[path] || '';
        }
    }

    GSettings {
        id: settings
        schema.id: "com.ubuntu.touch.system-settings"
    }
}
