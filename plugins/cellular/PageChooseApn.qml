/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Pat McGowan <pat.mcgowan@canonical.com>,
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
 *
 * Note: Everything user facing refers to APN and e.g. LTE, but in the
 * code an APN configuration is a 'context' and LTE is 'ia'.
 *
 */

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem
import MeeGo.QOfono 0.2
import Ubuntu.SystemSettings.Cellular 1.0
import "apn_manager.js" as Manager

ItemPage {
    id: root
    title: i18n.tr("APN")
    objectName: "apnPage"

    property var sim
    property var editor

    OfonoActivator {
        id: activator
    }

    /**
     * We have three ListModels: one for Internet contexts, one for MMS
     * contexts and one for ia contexts. We use OfonoConnectionContext qml
     * objects to represents the contexts.
     *
     * The model will have helpful properties:
     *     label: Label to put on the button that spawns the editor.
     *     title: Title that goes in the editor.
     *     type: A code that tells us what context type this model will have.
     *
     * Model objects will have the following roles:
     *     path: the path of the context
     *     qml: the QML of the context
     */

    ListModel {
        id: mmsContexts
        property string title: i18n.tr("MMS APN")
        property string type: 'mms'
    }

    ListModel {
        id: internetContexts
        property string title: i18n.tr("Internet APN")
        property string type: 'internet'
    }

    ListModel {
        id: iaContexts
        property string title: i18n.tr("LTE APN")
        property string type: 'ia'
    }

    Component {
        id: contextComponent
        OfonoContextConnection {
            property string typeString: {
                if (type === 'internet' && messageCenter) {
                    return i18n.tr("Internet and MMS");
                } else if (type === 'internet' && !messageCenter) {
                    return i18n.tr("Internet");
                } else if (type === 'ia') {
                    return i18n.tr("LTE");
                } else if (type === 'mms') {
                    return i18n.tr("MMS");
                } else {
                    return type;
                }
            }
        }
    }

    state: "default"
    states: [
        PageHeadState {
            name: "default"
            head: root.head
            actions: [
                Action {
                    iconName: "reset"
                    onTriggered: PopupUtils.open(resetDialog)
                },
                Action {
                    iconName: "add"
                    objectName: "newApn"
                    onTriggered: {
                        editor = pageStack.push(Qt.resolvedUrl("PageApnEditor.qml"), {
                            manager:         Manager,
                            mmsModel:        mmsContexts,
                            internetModel:   internetContexts,
                            iaModel:         iaContexts
                        });
                    }
                }
            ]
        }
    ]

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
            Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors { left: parent.left; right: parent.right }

            Repeater {
                id: apns
                anchors { left: parent.left; right: parent.right }
                height: childrenRect.height
                model: [internetContexts, mmsContexts, iaContexts]
                delegate: apnsDelegate
            }
        }
    }

    Component {
        id: resetDialog
        Dialog {
            id: dialogue
            title: i18n.tr("Reset APN Settings")
            text: i18n.tr("Are you sure that you want to Reset APN Settings?")

            Label {
                text: i18n.tr("The phone needs to restart for changes to take effect.")
                wrapMode: Text.WordWrap
            }

            Button {
                text: i18n.tr("Cancel")
                onClicked: PopupUtils.close(dialogue)
            }

            Button {
                text: i18n.tr("Reset")
                color: UbuntuColors.orange
                onClicked: {
                    Manager.reset();
                    PopupUtils.close(dialogue);
                }
            }
        }
    }

    Component {
        id: apnsDelegate

        Repeater {
            anchors { left: parent.left; right: parent.right }
            model: modelData
            delegate: apnDelegate
        }
    }

    Component {
        id: apnDelegate

        ListItem.Base {
            id: apnListItem
            property alias text: apnItemName.text
            objectName: "edit_" + qml.name
            height: units.gu(6)
            removable: true
            confirmRemoval: true
            progression: true

            onItemRemoved: Manager.removeContext(path);
            onClicked: {
                editor = pageStack.push(Qt.resolvedUrl("PageApnEditor.qml"), {
                    manager:         Manager,
                    contextQML:      qml,
                    mmsModel:        mmsContexts,
                    internetModel:   internetContexts,
                    iaModel:         iaContexts
                });
            }

            MouseArea {
                id: checkArea
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
                width: units.gu(5)
                height: units.gu(6)
                onClicked: check.trigger()

                CheckBox {
                    id: check
                    objectName: qml.accessPointName + "_preferred"
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }
                    property bool serverChecked: qml.preferred
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: {
                        if (checked) {
                            Manager.dePreferAll(qml.type);
                        }
                        qml.preferred = checked;
                    }
                }
            }

            Item  {
                id: middleVisuals
                anchors {
                    left: checkArea.right
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }

                height: childrenRect.height +
                        apnItemName.anchors.topMargin +
                        apnItemType.anchors.bottomMargin

                Label {
                    id: apnItemName
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }

                    text: qml.name
                    elide: Text.ElideRight
                    opacity: apnListItem.enabled ? 1.0 : 0.5
                }


                Label {
                    id: apnItemType
                    anchors {

                        left: parent.left
                        right: parent.right
                        top: apnItemName.bottom
                    }

                    text: qml.typeString
                    color: Theme.palette.normal.backgroundText
                    fontSize: "small"
                    wrapMode: Text.Wrap
                    maximumLineCount: 5
                }
            }
        }

    }

    Connections {
        target: sim.connMan

        onReportError: Manager.reportError(message)
        onContextRemoved: Manager.contextRemoved(path)
        onContextAdded: Manager.contextAdded(path)
        onContextsChanged: Manager.contextsChanged(contexts)
        Component.onCompleted: Manager.contextsChanged(sim.connMan.contexts)
    }
}
