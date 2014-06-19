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
    title: i18n.tr("Cellular")
    objectName: "cellularPage"

    OfonoRadioSettings {
        id: radioSettings
        modemPath: manager.modems[0]
        onTechnologyPreferenceChanged: {
            if(connMan.powered) {
                dataTypeSelector.selectedIndex = techPreference.keyToIndex(radioSettings.technologyPreference)
            }
            console.warn('connMan.powered',connMan.powered)
        }
    }

    OfonoManager {
        id: manager
    }

    OfonoSimManager {
        id: sim
        modemPath: manager.modems[0]
    }

    OfonoNetworkRegistration {
        id: netReg
        modemPath: manager.modems[0]
        property bool scanning: false
        onModeChanged: {
            if (mode === "manual")
                chooseCarrier.selectedIndex = 1;
            else
                chooseCarrier.selectedIndex = 0;
        }
    }

    OfonoConnMan {
        id: connMan
        modemPath: manager.modems[0]
        onPoweredChanged: console.warn('powered changed', connMan.powered)
    }

    property string carrierName: netReg.name

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListModel {
            id: techPreference

            ListElement { name: "Off"; key: "off" }
            ListElement { name: "2G only (saves battery)"; key: "gsm"; }
            ListElement { name: "2G/3G/4G (faster)"; key: "any"; }

            function keyToIndex (k) {
                console.warn('connMan.roamingAllowed', connMan.roamingAllowed);
                for (var i=0; i < techPreference.count; i++) {
                    if (techPreference.get(i).key === k) {
                        return i;
                    }
                }

                // make settings "lte", "umts" and all other unknown techs,
                // synonymous with the "any" setting
                return techPreference.get(techPreference.count - 1);
            }
        }

        Component {
            id: techPreferenceDelegate
            OptionSelectorDelegate { text: i18n.tr(name); }
        }

        ListItem.ItemSelector {
            id: dataTypeSelector
            objectName: "chooseDataTypeSelector"
            expanded: true
            delegate: techPreferenceDelegate
            model: techPreference
            selectedIndex: techPreference.keyToIndex(radioSettings.technologyPreference)
            onSelectedIndexChanged: {
                console.warn('onSelectedIndexChanged', selectedIndex, techPreference.get(selectedIndex).key)
                var key = techPreference.get(selectedIndex).key;
                if (key === 'off') {
                    connMan.powered = false;
                } else {
                    connMan.powered = true;
                    radioSettings.technologyPreference = key
                }

            }
        }

        ListItem.Standard {
            id: dataRoamingItem
            objectName: "dataRoamingSwitch"
            text: i18n.tr("Data roaming")
            // sensitive to data type, and disabled if "Off" is selected
            enabled: dataTypeSelector.selectedIndex !== 0
            control: Switch {
                id: dataRoamingControl
                checked: connMan.roamingAllowed
                onClicked: connMan.roamingAllowed = checked
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
            enabled: netReg.mode !== "auto-only"
            text: i18n.tr("Choose carrier:")
            model: [i18n.tr("Automatically"), i18n.tr("Manually")]
            selectedIndex: netReg.mode === "manual" ? 1 : 0
        }

        ListItem.SingleValue {
            text: i18n.tr("Carrier")
            objectName: "chooseCarrier"
            value: carrierName ? carrierName : i18n.tr("N/A")
            property bool enabled: chooseCarrier.selectedIndex === 1 // Manually
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
