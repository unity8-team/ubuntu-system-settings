/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem

ItemPage {
    id: root
    title: i18n.tr("Connection type")
    objectName: "connectionTypePage"

    property var sims

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
            spacing: units.gu(2)

            Repeater {
                id: repeater
                model: sims

                Column {

                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    spacing: 0

                    property var sim: modelData

                    SettingsItemTitle {
                        anchors { left: parent.left; right: parent.right }
                        text: sim.title
                        visible: repeater.count > 1

                        Label {
                            anchors {
                                right: parent.right
                                rightMargin: units.gu(2)
                                top: parent.top
                                topMargin: units.gu(3)
                            }
                            text: sim.netReg.name
                            opacity: 0.75
                        }
                    }

                    ListItem.ItemSelector {
                        id: radio

                        expanded: true
                        model: sim.radioSettings.modemTechnologies
                        delegate: OptionSelectorDelegate {
                            objectName: sim.path + "_radio_" + modelData
                            text: sim.techToString(modelData)
                        }
                        enabled: sim.radioSettings.technologyPreference !== ""
                        selectedIndex: sim.radioSettings.technologyPreference !== "" ?
                            model.indexOf(sim.radioSettings.technologyPreference) : -1

                        onDelegateClicked: {
                            if (model[index] === 'umts_enable') {
                                sim.radioSettings.technologyPreference = 'umts';
                                umtsModemChanged(sim, poweredSim ? poweredSim.path : "");
                                sim.mtkSettings.has3G = true;
                            } else {
                                sim.radioSettings.technologyPreference = model[index];
                            }
                        }

                        Connections {
                            target: sim.radioSettings
                            onTechnologyPreferenceChanged: radio.selectedIndex =
                                sim.radioSettings.modemTechnologies.indexOf(preference)

                            onModemTechnologiesChanged: {
                                if ((technologies.indexOf('umts') === -1)
                                     && (sim.mtkSettings.has3G === false)) {
                                    radio.model = sim.addUmtsEnableToModel(technologies);
                                } else {
                                    radio.model = technologies;
                                }
                                radio.selectedIndex = sim.radioSettings.technologyPreference !== "" ?
                                    model.indexOf(sim.radioSettings.technologyPreference) : -1
                            }
                            ignoreUnknownSignals: true
                        }

                        Component.onCompleted: {
                            if ((sim.radioSettings.modemTechnologies.indexOf('umts') === -1)
                                 && (sim.mtkSettings.has3G === false)) {
                                radio.model = sim.addUmtsEnableToModel(sim.radioSettings.modemTechnologies);
                            } else {
                                radio.model = sim.radioSettings.modemTechnologies;
                            }
                        }
                    }
                }
            }

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: units.gu(2)
                }

                fontSize: "small"
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
                text: i18n.tr("Faster connections may reduce battery life.")
            }
        }
    }
}
