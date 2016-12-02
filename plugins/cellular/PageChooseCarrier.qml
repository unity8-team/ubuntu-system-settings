/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
 *          Jonas G. Drange <jonas.drange@canonical.com>
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

import QtQuick 2.4
import QtQuick.Layouts 1.1
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import MeeGo.QOfono 0.2
import "carriers.js" as CHelper

ItemPage {
    id: root
    title: i18n.tr("Carrier")
    objectName: "chooseCarrierPage"
    flickable: scrollWidget

    property var sim
    property bool scanning: true
    property bool working: false

    states: [
        State {
            name: "auto"
            when: sim.netReg.mode === "auto"
            PropertyChanges {
                target: otherOperators
                selectedIndex: -1
            }
            PropertyChanges {
                target: allOperators
                height: 0
                opacity: 0
            }
        },
        State {
            name: "manual"
            when: sim.netReg.mode === "manual"
            PropertyChanges {
                target: otherOperators
                height: 0
                opacity: 0
            }
        }
        /* Note that we do not consider auto-only since this page is not
        reachable in that case (see Carrier & APN page). */
    ]

    Component {
        id: netOp
        OfonoNetworkOperator {
            onRegisterComplete: {
                if (error !== OfonoNetworkOperator.NoError) {
                    console.warn("Register complete:", errorString);
                    console.warn("Falling back to default operator.");
                    sim.netReg.registration();
                }
                working = false;
            }
        }
    }

    Connections {
        /* The following is a hack: If a scan is in progress and we call
        scan() on netReg, it emits a scanError _and_ scanFinished.
        We look at the scanError message, set this property to true if
        it says a scan is in progress. This way we can ignore the bad
        scanFinished signal. Filed bug against libqofono[1].
            [1] https://github.com/nemomobile/libqofono/issues/52
        */
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
        onModeChanged: modeSelector.selectedIndex = (mode === "auto") ? 0 : -1
    }

    Component.onCompleted: sim.netReg.scan()

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
            Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: 0

            SettingsItemTitle {
                text: i18n.tr("Carrier")

                ActivityIndicator {
                    anchors {
                        right: parent.right
                        top: parent.top
                        margins: units.gu(1.5)
                    }
                    running: true
                    opacity: scanning || working ? 1 : 0
                    Behavior on opacity {
                        NumberAnimation {
                            duration: UbuntuAnimation.SnapDuration
                        }
                    }
                }
            }

            ListItem.ItemSelector {
                id: modeSelector
                objectName: "mode"
                expanded: true
                enabled: !scanning && !working
                opacity: enabled ? 1 : 0.5
                model: [i18n.tr("Automatically")]
                delegate: OptionSelectorDelegate {
                    text: {
                        // modelData is "Automatically"
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

                /* When registration() fails, the UI state may end up in an
                undefined state. Issue[1] has been filed against libqofono.
                    [1] https://github.com/nemomobile/libqofono/issues/54
                */
                onDelegateClicked: sim.netReg.registration()
            }

            // In manual mode, all non-forbidden operators are shown.
            ListItem.ItemSelector {
                id: allOperators
                objectName: "allOperators"
                expanded: true
                enabled: !scanning && !working
                opacity: enabled ? 1 : 0.5
                model: CHelper.getOps(sim.netReg.networkOperators)
                delegate: OptionSelectorDelegate {
                    objectName: "carrier"
                    showDivider: false
                    text: modelData.name
                }
                onDelegateClicked: {
                    CHelper.setOp(model[index].operatorPath);
                    working = true;
                }
                selectedIndex: {
                    var curop = sim.netReg.currentOperatorPath;
                    return model.indexOf(CHelper.getOrCreateOpQml(curop));
                }

                Behavior on height {
                    NumberAnimation {
                        duration: UbuntuAnimation.SnapDuration
                    }
                }

                Behavior on opacity {
                    NumberAnimation {
                        duration: UbuntuAnimation.SnapDuration
                    }
                }
            }

            /* When registration mode is "Automatic", this list contains all
            the non-forbidden operators except the current one. When the user
            taps one of the elements in this selector, it will be hidden,
            and the mode will switch to "Manual". */
            ListItem.ItemSelector {
                id: otherOperators
                objectName: "otherOperators"
                expanded: true
                enabled: !scanning && !working
                opacity: enabled ? 1 : 0.5
                model: CHelper.getOps(sim.netReg.networkOperators,
                    [sim.netReg.currentOperatorPath])
                delegate: OptionSelectorDelegate {
                    objectName: "carrier"
                    showDivider: false
                    text: modelData.name
                }
                onDelegateClicked: {
                    var clickedOp = model[index].operatorPath;
                    CHelper.setOp(clickedOp);

                    // Update immediately and do not wait for netReg
                    allOperators.selectedIndex = allOperators.model.indexOf(
                        CHelper.getOrCreateOpQml(clickedOp));
                    working = true;
                }
                onSelectedIndexChanged: {
                    /* When e.g. the model changes, the selectedIndex is set to
                    0. Ignore this, since we never want the selectedIndex to be
                    anything other than -1. This component is shown only when
                    registration is "Automatic". */
                    if (selectedIndex >= 0) {
                        selectedIndex = -1;
                    }
                }

                Behavior on height {
                    NumberAnimation {
                        duration: UbuntuAnimation.SnapDuration
                    }
                }

                Behavior on opacity {
                    NumberAnimation {
                        duration: UbuntuAnimation.SnapDuration
                    }
                }
            }
        }
    }
}
