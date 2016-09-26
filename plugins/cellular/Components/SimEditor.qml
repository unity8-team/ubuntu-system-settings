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
import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

Column {
    id: simList
    objectName: "simEditor"

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
                text: sims[0].name
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
                text: sims[1].name
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

    NumberAnimation {
        id: scrollerAnimation
        duration: UbuntuAnimation.SnapDuration
        easing: UbuntuAnimation.StandardEasing
        target: root.flickable
        property: "contentY"
    }

    function openedEditor () {
        var flickable = scrollerAnimation.target;
        var maxFlick = Math.max(0, flickable.contentHeight - root.height);
        scrollerAnimation.from = flickable.contentY;
        scrollerAnimation.to = Math.min(y, maxFlick) - units.gu(9); // header
        scrollerAnimation.start();
        nameField.forceActiveFocus();
    }

    SettingsItemTitle { text: i18n.tr("Edit SIM Name") }

    ListItem.ExpandablesColumn {
        anchors {
            left: parent.left
            right: parent.right
        }
        height: expandedItem ?
            expandedItem.expandedHeight : childrenRect.height

        boundsBehavior: Flickable.StopAtBounds

        ListItem.Expandable {
            id: sim1Exp
            expandedHeight: sim1Col.height
            objectName: "edit_name_" + sims[0].path
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
                        objectName: "label_" + sims[0].path
                        anchors {
                            left: parent.left
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
                        text: sims[0].title
                    }
                }
                Column {
                    id: sim1Placeholder
                }
            }
            onClicked: {
                simList.state = "editingSim1";
                simList.openedEditor();
            }
        }

        ListItem.Expandable {
            id: sim2Exp
            expandedHeight: sim2Col.height
            objectName: "edit_name_" + sims[1].path
            showDivider: false
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
                        objectName: "label_" + sims[1].path
                        anchors {
                            left: parent.left
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
                        text: sims[1].title
                    }
                }
                Column {
                    id: sim2Placeholder
                }
            }
            onClicked: {
                simList.state = "editingSim2";
                simList.openedEditor();
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
            inputMethodHints: Qt.ImhNoPredictiveText
            onTriggered: renameAction
        }

        Row {

            spacing: units.gu(2)

            height: cancel.height + rename.height

            Button {
                id: cancel
                objectName: "cancelRename"
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
            height: units.gu(2)
            width: parent.width
        }

        Action {
            id: renameAction
            onTriggered: {
                var tmpSimNames = {};
                if (simList.state === "editingSim1") {
                    tmpSimNames[sims[0].path] = nameField.text;
                    tmpSimNames[sims[1].path] = sims[1].name;
                } else if (simList.state === "editingSim2") {
                    tmpSimNames[sims[0].path] = sims[0].name;
                    tmpSimNames[sims[1].path] = nameField.text;
                }
                phoneSettings.simNames = tmpSimNames;
                simList.state = "";
            }
        }
    }
}

