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
import Ubuntu.SystemSettings.Phone 1.0

ItemPage {
    title: i18n.tr("Cellular")
    objectName: "cellularPage"

    NetworkRegistration {
        id: netReg
        onModeChanged: {
            if (mode === "manual")
                chooseCarrier.selectedIndex = 1;
            else
                chooseCarrier.selectedIndex = 0;
        }
        Component.onCompleted: {
            /* NetworkRegistration provides an enum for data technology,
             * including:
             *     UnknownDataTechnology
             *     GprsDataTechnology
             *     EdgeDataTechnology
             *     UmtsDataTechnology
             *     HspaDataTechnology
             */
            if (technology == NetworkRegistration.UnknownDataTechnology)
                console.log ("Unknown data technology");
        }
    }

    ConnMan {
        id: connMan
    }

    property string carrierName: netReg.name

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        /* TODO: use selector once ofono supports those options (bug #1211804) */
        ListItem.ItemSelector {
            id: dataTypeSelector
            expanded: true
            visible: showAllUI
            text: i18n.tr("Cellular data:")
            model: [i18n.tr("Off"),
                i18n.tr("2G only (saves battery)"),
                i18n.tr("2G/3G/4G (faster)")]
            selectedIndex: !connMan.powered ? 0 : 2
            onSelectedIndexChanged: {
                if (selectedIndex == 0)
                    connMan.powered = false;
                else
                    connMan.powered = true;
            }
        }

        ListItem.Standard {
            text: i18n.tr("Cellular data")
            visible: !showAllUI
            control: Switch {
                id: dataSwitch
                checked: connMan.powered
                onClicked: connMan.powered = checked
            }
         }

        ListItem.Standard {
            id: dataRoamingItem
            text: i18n.tr("Data roaming")
            enabled: dataSwitch.checked
            control: Switch {
                id: dataRoamingControl
                checked: connMan.roamingAllowed
                onClicked: connMan.roamingAllowed = checked
            }
            onEnabledChanged: {
                if (!enabled)
                    dataRoamingControl.checked = false
                else
                    dataRoamingControl.checked = Qt.binding(function() {
                        return connMan.roamingAllowed
                    })
            }
        }

        ListItem.SingleValue {
            text: i18n.tr("Wifi hotspot")
            onClicked: {
                pageStack.push(Qt.resolvedUrl("Hotspot.qml"))
            }
        }

        ListItem.Standard {
            text: i18n.tr("Data usage statistics")
            progression: true
            visible: showAllUI
        }

        ListItem.ItemSelector {
            id: chooseCarrier
            objectName: "autoChooseCarrierSelector"
            expanded: true
            enabled: netReg.mode != "auto-only"
            text: i18n.tr("Choose carrier:")
            model: [i18n.tr("Automatically"), i18n.tr("Manually")]
            selectedIndex: netReg.mode == "manual" ? 1 : 0
        }

        ListItem.SingleValue {
            text: i18n.tr("Carrier")
            objectName: "chooseCarrier"
            value: carrierName ? carrierName : i18n.tr("N/A")
            property bool enabled: chooseCarrier.selectedIndex == 1 // Manually
            progression: enabled
            onClicked: {
                if (enabled)
                    pageStack.push(Qt.resolvedUrl("ChooseCarrier.qml"), {netReg: netReg})
            }
        }

        ListItem.Standard {
            text: i18n.tr("APN")
            progression: true
            visible: showAllUI
        }
    }
}
