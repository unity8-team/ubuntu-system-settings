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
import QtQuick.Layouts 1.1
import SystemSettings 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import MeeGo.QOfono 0.2

ItemPage {
    id: root
    title: i18n.tr("Carrier")

    objectName: "chooseCarrierPage"

    property var sim

    property variant operatorNames
    property int mode

    QtObject {
        id: d
        property bool __suppressActivation : true;
    }

    Component.onCompleted: {
        updateNetworkOperators();
    }

    Connections {
        target: sim.netReg
        onNetworkOperatorsChanged: updateNetworkOperators();
        onCurrentOperatorPathChanged: buildLists();
    }

    // map of operatorPath : netOp
    // FIXME: comment on operators' value
    property var operators: ({})
    function updateNetworkOperators()
    {
        var tmp = sim.netReg.networkOperators;
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
        d.__suppressActivation = true;
        var oN = new Array();

        for (var i in operators) {
            var tempOp = operators[i];
            if (tempOp.status === "forbidden")
                continue
            oN.push(tempOp.name);
        }
        operatorNames = oN;

        var cur = operators[sim.netReg.currentOperatorPath];
        carrierSelector.selectedIndex = cur === undefined ? -1 : operatorNames.indexOf(cur.name);
        d.__suppressActivation = false;
    }

    Component {
        id: netOp
        OfonoNetworkOperator {
            onRegisterComplete: {
                if (error === OfonoNetworkOperator.InProgressError) {
                    console.warn("registerComplete failed with error: " + errorString);
                } else if (error !== OfonoNetworkOperator.NoError) {
                    console.warn("registerComplete failed with error: " + errorString + " Falling back to default");
                    sim.netReg.registration();
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

        ColumnLayout {

            anchors {
                left: parent.left
                right: parent.right
            }

            ListItem.ItemSelector {
                id: chooseCarrier
                objectName: "autoChooseCarrierSelector"
                expanded: true
                enabled: sim.netReg.mode !== "auto-only"
                text: i18n.tr("Choose carrier:")
                model: [i18n.tr("Automatically"), i18n.tr("Manually")]

                delegate: OptionSelectorDelegate { showDivider: false }
                selectedIndex: sim.netReg.mode === "manual" ? 1 : 0

                // we only want to do this per user input
                onSelectedIndexChanged: {
                    if (selectedIndex === -1 || d.__suppressActivation)
                        return;

                    if (selectedIndex === 0) {
                        sim.netReg.registration();
                    } else if (selectedIndex === 1) {
                        if (sim.netReg.status !== "searching")
                            sim.netReg.scan();
                    }
                }
            }
            ListItem.SingleControl {
                enabled: chooseCarrier.selectedIndex === 1
                anchors {
                    left: parent.left
                    leftMargin: units.gu(0)
                }
                control: ColumnLayout {
                    id: child
                    width: parent.width - units.gu(4)
                    anchors.left: parent.left
                    RowLayout {
                        id: searchingRow
                        spacing: units.gu(1)

                        visible: sim.netReg.status === "searching"
                        ActivityIndicator {
                            id: activityIndicator
                            anchors.verticalCenter: parent.verticalCenter
                            running: parent.visible
                        }
                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            text: i18n.tr("Searching for carriers…")
                        }
                    }
                    ListItem.ItemSelector {
                        id: carrierSelector
                        objectName: "carrierSelector"
                        expanded: true
                        enabled: sim.netReg.status !== "searching" && chooseCarrier.selectedIndex === 1
                        // work around ItemSelector not having a visual change depending on being disabled
                        opacity: enabled ? 1.0 : 0.5
                        width: parent.width
                        model: operatorNames
                        delegate: OptionSelectorDelegate { enabled: carrierSelector.enabled; showDivider: false }
                        onSelectedIndexChanged: {
                            if (selectedIndex === -1 || d.__suppressActivation)
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
                }
            }
            ListItem.Standard {
                text: i18n.tr("APN")
                progression: true
                enabled: sim.connMan.powered
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("PageChooseApn.qml"), {sim: sim})
                }
            }
        }
    }
}
