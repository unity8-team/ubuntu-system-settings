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

    anchors.left: parent.left
    anchors.right: parent.right

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
        }
    ]

    ListItem.Standard {
        text: i18n.tr("Edit SIM Name")
    }


    ListItem.Standard {
        id: sim1Item
        text: sim1.title
        objectName: "editSim1"
        progression: true
        onClicked: {
            simList.state = "editingSim1";
            nameField.forceActiveFocus();
        }
    }

    Column {
        id: sim1Placeholder
    }

    ListItem.Standard {
        id: sim2Item
        text: sim2.title
        objectName: "editSim2"
        progression: true
        onClicked: {
            simList.state = "editingSim2";
            nameField.forceActiveFocus();
        }
    }

    Column {
        id: sim2Placeholder
    }

    Column {
        id: editor
        visible: false

        width: simList.width - units.gu(4)
        spacing: units.gu(2)
        anchors {
            horizontalCenter: simList.horizontalCenter
        }

        Item {
            height: units.gu(0.1)
            width: parent.width
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

        Action {
            id: renameAction
            onTriggered: {
                if (simList.state === "editingSim1") {
                    //ussS.sim1Name = nameField.text;
                } else if (simList.state === "editingSim2") {
                    //ussS.sim2Name = nameField.text;
                }
                simList.state = "";
            }
        }
    }
}

