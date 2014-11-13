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
        /* The following is a hack: If a scan is in progress and we call
        scan() on netReg, it emits and scanError _and_ scanFinished.
        We look at the scanError message, set this property to true if
        it says a scan is in progress. This way we can ignore the bad
        scanFinished signal. Filed bug against libqofono. */
        property bool __scanInProgress: false

        target: sim.netReg
        onScanFinished: {
            if (scanning && !__scanInProgress) {
                scanning = false;
            }
            __scanInProgress = false;
        }
        onScanError: {
            if (message === "Operation already in progress") {
                console.warn('A scan was already in progress.');
                __scanInProgress = true;
            } else {
                scanning = false;
                __scanInProgress = false;
                console.warn("onScanError: " + message);
            }
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
            id: col
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

            SettingsItemTitle { text: i18n.tr("Choose carrier:") }

            ListItem.ItemSelector {
                id: chooseCarrier
                objectName: "mode"
                expanded: true
                enabled: !scanning
                opacity: enabled ? 1 : 0.5
                model: [i18n.tr("Automatically")]
                delegate: OptionSelectorDelegate {
                    text: {
                        if (sim.netReg.mode === "auto") {
                            return sim.netReg.name ?
                                modelData + " [ " + sim.netReg.name + " ]" :
                                    modelData
                        } else {
                            return modelData;
                        }
                    }
                    showDivider: false
                }
                selectedIndex: sim.netReg.mode === "auto" ? 0 : -1
                onDelegateClicked: sim.netReg.registration()

                ActivityIndicator {
                    id: act
                    anchors {
                        right: parent.right
                        top: parent.top
                        margins: units.gu(1.5)
                    }
                    running: true
                    opacity: scanning ? 1 : 0
                    Behavior on opacity {
                        NumberAnimation {
                            duration: UbuntuAnimation.SnapDuration
                        }
                    }
                }
            }

            ListItem.ItemSelector {
                id: allOperators
                objectName: "allOperators"
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
                objectName: "otherOperators"
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
        }
    }
}
