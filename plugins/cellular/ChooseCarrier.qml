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


ItemPage {
    title: i18n.tr("Carrier")
    property var netreg
    property variant operators: netreg.operators
    property bool scanning: netreg.scanning
    property variant operatorNames
    property variant operatorStatus
    property int curOp

    Component.onCompleted: { netreg.getOperators(); console.log ("operators: " + operators); }

    onOperatorsChanged: {
        console.log("netreg.onOperatorsChanged");
        buildLists();
    }


    function buildLists()
    {
        var oN = new Array();
        var oS = new Array();
        for (var i in operators)
        {
            console.log ("HERE: " + operators[i].name);
            oN.push(operators[i].name);
            oS.push(operators[i].status);
        }
        curOp = oS.indexOf("current");
        operatorNames = oN;
        operatorStatus = oS;
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

           ListItem.ValueSelector {
                id: carrierSelector
                expanded: true
                // TODO: There is no way to have a ValueSelector always expanded
                onExpandedChanged: expanded = true
                /* FIXME: This is disabled since it is currently a
                 * read-only setting
                 * enabled: cellularDataControl.checked
                 */
                enabled: true
                values: operatorNames
                selectedIndex: curOp
                onSelectedIndexChanged: {
                    console.log ("CHANGED: " + operators[selectedIndex].name);
                }
            }
    }


    ListItem.SingleControl {
        anchors.bottom: parent.bottom
        control: Button {
            width: parent.width - units.gu(4)
            text: i18n.tr("Scan")
            onTriggered: {
                console.log ("Scanning");
                netreg.scan();
            }
        }
    }

    ActivityIndicator {
        id: activityIndicator
        anchors.centerIn: parent
        running: scanning
    }

    Text {
        anchors {
            top: activityIndicator.bottom
            topMargin: units.gu(2)
            horizontalCenter: activityIndicator.horizontalCenter
        }
        text: i18n.tr("Searching")
        visible: activityIndicator.running
    }
}


