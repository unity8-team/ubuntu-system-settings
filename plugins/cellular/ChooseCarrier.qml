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
    property var operators: []
    property bool scanning: false
    property variant operatorNames
    property variant operatorStatus
    property int curOp
    property int mode

    mode: netReg.mode === "manual" ? 1 : 0

    Component.onCompleted: buildLists();

    Connections {
        target: netReg
        onStatusChanged: {
            console.warn("onStatusChanged: " + netReg.status);
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
        onNetworkOperatorsChanged: buildLists();
        onScanFinished: scanning = false;
        onScanError: {
            scanning = false;
            console.warn ("onScanError: " + message);
        }
    }

    Connections {
        target: connMan
    }

    function buildLists()
    {
        var ops = [];
        var oN = new Array();
        var oS = new Array();
        for (var i = 0; i < netReg.networkOperators.length; i++) {
            var tempOp = netOp.createObject(parent, {"operatorPath": netReg.networkOperators[i]});
            if (tempOp.status === "forbidden")
                continue
            oN.push(tempOp.name);
            oS.push(tempOp.status);
            ops.push(tempOp)
        }
        curOp = oS.indexOf("current");
        operatorNames = oN;
        operatorStatus = oS;
        operators = ops;
        carrierSelector.selectedIndex = curOp;
    }

    Component {
        id: netOp
        OfonoNetworkOperator {
            onRegisterComplete: {
                if (error === OfonoNetworkOperator.NoError)
                    console.warn("registerComplete: SUCCESS");
                else if (error === OfonoNetworkOperator.InProgressError)
                    console.warn("registerComplete failed with error: " + errorString);
                else {
                    console.warn("registerComplete failed with error: " + errorString + " Falling back to default");
                    netReg.registration();
                }
            }
        }
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick

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

            //                property bool enabled: chooseCarrier.selectedIndex === 1 // Manually

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
                onSelectedIndexChanged: {
                    if ((selectedIndex !== curOp) && operators[selectedIndex]) {
                        console.warn("onSelectedIndexChanged status: " + operators[selectedIndex].status);
                        operators[selectedIndex].registerOperator();
                    }
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
