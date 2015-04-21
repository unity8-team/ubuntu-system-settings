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
import "apn.js" as APN

ItemPage {
    id: root
    title: i18n.tr("APN")
    objectName: "apnPage"

    property var sim
    property var apnEditor

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
     *     current: Falsy if no context is active, and a OfonoConnectionContext
     *              if one is active.
     *
     * Model objects will have the following roles:
     *     path: the path of the context
     *     qml: the QML of the context
     */

    ListModel {
        id: mmsContexts
        property string label: i18n.tr('MMS APN…')
        property string title: i18n.tr("MMS APN")
        property string type: 'mms'
        property var current
    }

    ListModel {
        id: internetContexts
        property string label: i18n.tr('Internet APN…')
        property string title: i18n.tr("Internet APN")
        property string type: 'internet'
        property var current
    }

    ListModel {
        id: iaContexts
        property string label: i18n.tr('LTE APN…')
        property string title: i18n.tr("LTE APN")
        property string type: 'ia'
        property var current
    }

    Component {
        id: contextComponent
        OfonoContextConnection {}
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
            Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors { left: parent.left; right: parent.right }

            /* This repeater creates three list items: MMS, Internet and LTE. */
            Repeater {
                id: apns
                anchors { left: parent.left; right: parent.right }
                height: childrenRect.height
                model: [mmsContexts, internetContexts, iaContexts]
                delegate: apnsDelegate
            }

            Item {
                anchors { left: parent.left; right: parent.right; }
                height: units.gu(2)
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - units.gu(4)
                text: i18n.tr("Reset APN Settings")
                onTriggered: PopupUtils.open(resetDialog)
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
                    APN.reset();
                    PopupUtils.close(dialogue);
                }
            }
        }
    }

    Component {
        id: editor

        CustomApnEditor {
            id: editorDialog

            onActivated: {
                console.warn('activated in editor');
            }

            onCanceled: {
                PopupUtils.close(apnEditor);
            }
        }
    }

    Component {
        id: apnsDelegate
        ListItem.Subtitled {
            text: modelData.label
            subText: modelData.current ?
                modelData.current.accessPointName : i18n.tr('Not set')
            onClicked: apnEditor = PopupUtils.open(editor, root, {
                contextModel:    modelData,
                mmsModel:        mmsContexts,
                internetModel:   internetContexts,
                iaModel:         iaContexts,
                apnLib:          APN,
                title:           modelData.title
            });
        }
    }

    Connections {
        target: sim.connMan

        onReportError: APN.reportError(message)
        onContextRemoved: APN.contextRemoved(path)
        onContextAdded: APN.contextAdded(path)
        onContextsChanged: APN.contextsChanged(contexts)
        Component.onCompleted: APN.contextsChanged(sim.connMan.contexts)
    }
}
