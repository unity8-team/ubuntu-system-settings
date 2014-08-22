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
    id: root
    title: i18n.tr("Carrier")

    objectName: "chooseCarrierPage"

    property var netReg
    property var connMan
    property bool scanning: false
    property variant operatorNames
    property int mode

    mode: netReg.mode === "manual" ? 1 : 0

    Component.onCompleted: {
        updateNetworkOperators();
    }

    Connections {
        target: netReg
        onStatusChanged: {
            if (netReg.status === "registered")
                buildLists();
        }
        onModeChanged: {
            console.warn ("onModeChanged: " + mode);
            if (mode === "manual")
                chooseCarrier.selectedIndex = 1;
            else
                chooseCarrier.selectedIndex = 0;
        }
        onNetworkOperatorsChanged: updateNetworkOperators();
        onScanFinished: scanning = false;
        onScanError: {
            scanning = false;
            console.warn ("onScanError: " + message);
        }
    }

    Connections {
        target: connMan
    }

    // map of operatorPath : netOp
    property var operators: ({}) // naughty, naughty
    function updateNetworkOperators()
    {
        var tmp = netReg.networkOperators;
        var added = tmp.filter(function(i) {
            return operators[i] === undefined;
        });
        var removed = Object.keys(operators).filter(function(i) {
            return tmp.indexOf(i) === -1;
        })

        removed.forEach(function(currentValue, index, array) {
            // just asserting to verify the logic
            // remove once proven functional
            if (operators[currentValue] === undefined) {
                throw "updateNetworkOperators: removed is broken";
            }

            operators[currentValue].destroy();
            delete operators[currentValue];
        });

        added.forEach(function(currentValue, index, array) {
            // just asserting to verify the logic
            // remove once proven functional
            if (operators[currentValue] !== undefined) {
                throw "updateNetworkOperators: added is broken";
            }

            operators[currentValue] = netOp.createObject(parent,
                                                         {
                                                             "operatorPath": currentValue
                                                         });
        });

        // just asserting to verify the logic
        // remove once proven functional
        if (Object.keys(operators).length !== tmp.length) {
            throw "Object.keys(operators).length !== tmp.length";
        }
        tmp.forEach(function(currentValue, index, array) {
            if (operators[currentValue] === undefined)
                throw "operators[currentValue] === undefined";
        });

        buildLists();
    }

    function buildLists()
    {
        var oN = new Array();

        for (var i in operators) {
            var tempOp = operators[i];
            if (tempOp.status === "forbidden")
                continue
            oN.push(tempOp.name);
        }
        operatorNames = oN;

        var cur = operators[netReg.currentOperatorPath];
        carrierSelector.selectedIndex = cur === undefined ? -1 : operatorNames.indexOf(cur.name);
    }

    Component {
        id: netOp
        OfonoNetworkOperator {
            onRegisterComplete: {
                if (error === OfonoNetworkOperator.InProgressError) {
                    console.warn("registerComplete failed with error: " + errorString);
                } else if (error !== OfonoNetworkOperator.NoError) {
                    console.warn("registerComplete failed with error: " + errorString + " Falling back to default");
                    netReg.registration();
                }
            }
            onNameChanged:  buildLists();
            onStatusChanged: buildLists();
        }        
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: parent.height
        boundsBehavior: (contentHeight > parent.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.ItemSelector {
                id: chooseCarrier
                objectName: "autoChooseCarrierSelector"
                expanded: true
                enabled: netReg.mode !== "auto-only"
                text: i18n.tr("Choose carrier:")
                model: [i18n.tr("Automatically"), i18n.tr("Manually")]

                //showDivider: false
                delegate: OptionSelectorDelegate { showDivider: false }
                selectedIndex: netReg.mode === "manual" ? 1 : 0
                onSelectedIndexChanged: {
                    if (selectedIndex === 0)
                        netReg.registration();
                }
            }

            ListItem.ItemSelector {
                id: carrierSelector
                objectName: "carrierSelector"
                expanded: enabled
                enabled: chooseCarrier.selectedIndex === 1
                model: operatorNames
                onSelectedIndexChanged: {
                    if (selectedIndex === -1)
                        return;

                    // this assumes operator names are unique,
                    // revise if not so
                    for (var op in operators) {
                        if (operators[op].name === operatorNames[selectedIndex]) {
                            operators[op].registerOperator();
                            return;
                        }
                    }
                    // just asserting to verify the logic
                    // remove once proven functional
                    throw "should not be reached.";
                }
            }
            
            ListItem.Standard {
                text: i18n.tr("APN")
                progression: true
                visible: true
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("Apn.qml"), {connMan: connMan})
                }
            }
        }

        ListItem.SingleControl {
            anchors.bottom: parent.bottom
            control: Button {
                objectName: "refreshButton"
                width: parent.width - units.gu(4)
                text: i18n.tr("Refresh")
                enabled: (netReg.status !== "searching") && (netReg.status !== "denied")
                onTriggered: {
                    scanning = true;
                    netReg.scan();
                }
            }
        }

        ListItem.SingleControl {
            anchors.bottom: parent.bottom
            control: Button {
                objectName: "refreshButton"
                width: parent.width - units.gu(4)
                text: i18n.tr("Refresh")
                enabled: (netReg.status !== "searching") && (netReg.status !== "denied")
                onTriggered: {
                    scanning = true;
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
}
