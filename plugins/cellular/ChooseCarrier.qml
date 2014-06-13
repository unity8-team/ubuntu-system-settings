/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import MeeGo.QOfono 0.2

ItemPage {
    title: i18n.tr("Carrier")
    objectName: "chooseCarrierPage"

    property var netReg
    property variant operators: []
    property bool scanning: netReg.scanning
    property variant operatorNames
    property variant operatorStatus
    property int curOp

    Connections {
        target: netReg
        onNetworkOperatorsChanged: {
            buildLists();
        }
        onScanFinished: netReg.scanning = false;
    }

    function buildLists()
    {
        operators = [];
        var oN = new Array();
        var oS = new Array();
        for (var i in netReg.networkOperators)
        {
            var tempOp = netOp.createObject(parent, {"operatorPath": netReg.networkOperators[i]});

            if (tempOp.status === "forbidden")
                continue
            oN.push(tempOp.name);
            oS.push(tempOp.status);
            operators.push(tempOp)
        }
        curOp = oS.indexOf("current");
        operatorNames = oN;
        operatorStatus = oS;
    }

    Component {
        id: netOp
        OfonoNetworkOperator {
            modemPath: netReg.modemPath
            onRegisterComplete: {
                print ("registerComplete: " + errorString);
            }
        }
    }

    ListItem.ItemSelector {
        id: carrierSelector
        objectName: "carrierSelector"
        expanded: true
        /* FIXME: This is disabled since it is currently a
         * read-only setting
         * enabled: cellularDataControl.checked
         */
        enabled: true
        model: operatorNames
        selectedIndex: curOp
        onSelectedIndexChanged: {
            netOp.operatorPath =
            operators[selectedIndex].registerOperator();
        }
    }

    ListItem.SingleControl {
        anchors.bottom: parent.bottom
        control: Button {
            objectName: "refreshButton"
            width: parent.width - units.gu(4)
            text: i18n.tr("Refresh")
            onTriggered: {
                netReg.scanning = true;
                netReg.scan();
            }
        }
    }

    ActivityIndicator {
        id: activityIndicator
        anchors.centerIn: parent
        running: scanning
    }

    Label {
        anchors {
            top: activityIndicator.bottom
            topMargin: units.gu(2)
            horizontalCenter: activityIndicator.horizontalCenter
        }
        text: i18n.tr("Searching")
        visible: activityIndicator.running
    }
}
