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
import "Components" as LocalComponents
import "carriers.js" as CHelper

ItemPage {
    id: root
    title: i18n.tr("Carrier")
    objectName: "chooseCarrierPage"

    property var sim
    property bool scanning: true

    states: [
        State {
            name: "auto"
            when: sim.netReg.mode === "auto"
            PropertyChanges {
                target: otherOperators
                visible: true
                selectedIndex: -1
            }
            PropertyChanges {
                target: curOpLabel
                height: chooseCarrier.itemHeight
            }
        },
        State {
            name: "manual"
            when: sim.netReg.mode === "manual"
            PropertyChanges {
                target: allOperators
                visible: true
            }
        },
        State {
            name: "auto-only"
            when: sim.netReg.mode === "auto-only"
            PropertyChanges {
                target: chooseCarrier
                enabled: false
            }
        }
    ]

    Component {
        id: netOp
        OfonoNetworkOperator {
            onRegisterComplete: {
                if (error === OfonoNetworkOperator.InProgressError) {
                    console.warn("Register failed, already one in progress.");
                    console.warn("Falling back to default operator.");
                    sim.netReg.registration();
                } else if (error !== OfonoNetworkOperator.NoError) {
                    console.warn("Register complete:", errorString);
                    console.warn("Falling back to default operator.");
                    sim.netReg.registration();
                }
            }
        }
    }

    Connections {
        target: sim.netReg
        onScanFinished: {
            if (scanning) {
                scanning = false;
            }
        }
        onScanError: {
            scanning = false;
            console.warn("onScanError: " + message);
        }
        onModeChanged: chooseCarrier.selectedIndex = (mode === "auto") ? 0 : -1
    }

    Component.onCompleted: sim.netReg.scan()

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: parent.height
        boundsBehavior: (contentHeight > parent.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            move: Transition {
                 NumberAnimation {
                     properties: "y"
                     duration: UbuntuAnimation.SnapDuration
                 }
             }
            spacing: 0

            Label{
                text: "state:" + root.state + " status: " + sim.netReg.status
            }

            ListItem.ItemSelector {
                id: chooseCarrier
                objectName: "mode"
                expanded: true
                enabled: !scanning
                opacity: enabled ? 1 : 0.5
                text: i18n.tr("Choose carrier:")
                model: [i18n.tr("Automatically")]
                delegate: OptionSelectorDelegate { showDivider: false }
                selectedIndex: sim.netReg.mode === "auto" ? 0 : -1
                onDelegateClicked: sim.netReg.registration()
            }


            LocalComponents.OperatorLabel {
                id: curOpLabel
                operatorName: sim.netReg.name
            }

            ListItem.ItemSelector {
                id: allOperators
                expanded: true
                enabled: !scanning
                opacity: enabled ? 1 : 0.5
                visible: false
                model: CHelper.getOps(sim.netReg.networkOperators)
                delegate: OptionSelectorDelegate {
                    objectName: "carrier"
                    showDivider: false
                    text: modelData.name
                }
                onDelegateClicked: CHelper.setOp(model[index].operatorPath)
                selectedIndex: {
                    var curop = sim.netReg.currentOperatorPath;
                    return model.indexOf(CHelper.getOrCreateOpQml(curop));
                }
            }

            ListItem.ItemSelector {
                id: otherOperators
                expanded: true
                visible: false
                enabled: !scanning
                opacity: enabled ? 1 : 0.5
                model: CHelper.getOps(sim.netReg.networkOperators,
                    [sim.netReg.currentOperatorPath])
                delegate: OptionSelectorDelegate {
                    objectName: "carrier"
                    showDivider: false
                    text: modelData.name
                }
                onDelegateClicked: CHelper.setOp(model[index].operatorPath)
                selectedIndex: -1
            }

            LocalComponents.ListItemIndicator {
                id: scanIndicator
                running: scanning
                text: i18n.tr("Searching for carriers…")
                height: scanning ? chooseCarrier.itemHeight : 0
            }
        }
    }
}
