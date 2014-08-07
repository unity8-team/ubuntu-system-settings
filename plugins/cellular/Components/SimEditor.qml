/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/
import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Column {

    id: simList
    objectName: "simEditor"

    anchors {
        left: parent.left
        right: parent.right
    }

    states: [
        State {
            name: "editing"
            PropertyChanges {
                target: editor
                visible: true
            }
        },
        State {
            extend: "editing"
            name: "editingSim1"
            PropertyChanges {
                target: nameField
                text: sim1.name
            }
            ParentChange {
                target: editor
                parent: sim1Placeholder
            }
            PropertyChanges {
                target: sim1Exp
                expanded: true
            }
        },
        State {
            extend: "editing"
            name: "editingSim2"
            PropertyChanges {
                target: nameField
                text: sim2.name
            }
            ParentChange {
                target: editor
                parent: sim2Placeholder
            }
            PropertyChanges {
                target: sim2Exp
                expanded: true
            }
        }
    ]

    ListItem.Standard {
        text: i18n.tr("Edit SIM Name")
    }

    ListItem.ExpandablesColumn {
        anchors {
            left: parent.left
            right: parent.right
        }
        height: expandedItem ?
            childrenRect.height + editor.height : childrenRect.height

        ListItem.Expandable {
            id: sim1Exp
            expandedHeight: sim1Col.height
            objectName: "editSim1"
            Column {
                id: sim1Col
                anchors {
                    left: parent.left
                    right: parent.right
                }
                Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: sim1Exp.collapsedHeight
                    Label {
                        objectName: "simLabel1"
                        anchors {
                            left: parent.left
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
                        text: sim1.title
                    }
                }
                Column {
                    id: sim1Placeholder
                }
            }
            onClicked: {
                simList.state = "editingSim1";
                nameField.forceActiveFocus();
            }
        }

        ListItem.Expandable {
            id: sim2Exp
            expandedHeight: sim2Col.height
            objectName: "editSim2"
            Column {
                id: sim2Col
                anchors {
                    left: parent.left
                    right: parent.right
                }
                Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: sim2Exp.collapsedHeight
                    Label {
                        objectName: "simLabel2"
                        anchors {
                            left: parent.left
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
                        text: sim2.title
                    }
                }
                Column {
                    id: sim2Placeholder
                }
            }
            onClicked: {
                simList.state = "editingSim2";
                nameField.forceActiveFocus();
            }
        }
    }

    // this column will be re-parented by a simList state change
    Column {
        id: editor
        visible: false

        width: simList.width - units.gu(4)
        spacing: units.gu(2)
        anchors {
            horizontalCenter: simList.horizontalCenter
        }

        TextField {
            id: nameField
            objectName: "nameField"
            maximumLength: 30
            width: simList.width - units.gu(4)
            onTriggered: renameAction
        }

        Row {

            spacing: units.gu(2)

            Button {
                id: cancel
                objectName: "cancelRename"
                gradient: UbuntuColors.greyGradient
                text: i18n.tr("Cancel")
                width: (editor.width / 2) - units.gu(1)
                onClicked: {
                    simList.state = "";
                }
            }

            Button {
                id: rename
                objectName: "doRename"
                enabled: nameField.text
                text: i18n.tr("OK")
                width: (editor.width / 2) - units.gu(1)
                action: renameAction
            }
        }

        Item {
            height: units.gu(1)
            width: parent.width
        }

        Action {
            id: renameAction
            onTriggered: {
                var tmpSimNames = {};
                if (simList.state === "editingSim1") {
                    tmpSimNames[sim1.path] = nameField.text;
                    tmpSimNames[sim2.path] = sim2.name;
                } else if (simList.state === "editingSim2") {
                    tmpSimNames[sim1.path] = sim1.name;
                    tmpSimNames[sim2.path] = nameField.text;
                }
                phoneSettings.simNames = tmpSimNames;
                simList.state = "";
            }
        }
    }
}

