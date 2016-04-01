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

import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import MeeGo.QOfono 0.2
import Ubuntu.SystemSettings.Cellular 1.0
import "apn_manager.js" as Manager

ItemPage {
    id: root
    title: i18n.tr("APN")
    objectName: "apnPage"

    property var sim
    property var editor

    // Signal that indicates that we have all our contexts.
    signal ready()
    Component.onCompleted: root.ready.connect(Manager.ready)

    /**
     * We have three ListModels: one for Internet contexts, one for MMS
     * contexts and one for ia contexts. We use OfonoContextConnection qml
     * objects to represents the contexts.
     *
     * The model will have helpful properties:
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
            property bool isCombined: type === 'internet' && messageCenter
            property string typeString: {
                if (isCombined) {
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
                    iconName: "add"
                    objectName: "newApn"
                    onTriggered: {
                        editor = pageStack.push(pageApnEditor, {
                            mmsModel:        mmsContexts,
                            internetModel:   internetContexts,
                            iaModel:         iaContexts
                        });
                    }
                }
            ]
        }
    ]

    Component {
        id: pageApnEditor
        PageApnEditor {
            onRequestContextCreation: Manager.createContext(type)
        }
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
            Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            id: apnsCol
            anchors { left: parent.left; right: parent.right }

            Repeater {
                id: apns
                anchors { left: parent.left; right: parent.right }
                height: childrenRect.height
                model: [internetContexts, mmsContexts, iaContexts]
                delegate: apnsDelegate
            }
        }

        Button {
            anchors {
                top: apnsCol.bottom
                right: parent.right
                left: parent.left
                margins: units.gu(2)
            }
            text: i18n.tr("Reset All APN Settings…")
            onClicked: PopupUtils.open(resetDialog)
        }
    }

    Component {
        id: resetDialog
        Dialog {
            id: dialogue
            title: i18n.tr("Reset APN Settings")
            text: i18n.tr("Are you sure that you want to Reset APN Settings?")

            Button {
                text: i18n.tr("Reset")
                color: UbuntuColors.orange
                onClicked: {
                    Manager.reset();
                    PopupUtils.close(dialogue);
                }
            }

            Button {
                text: i18n.tr("Cancel")
                onClicked: PopupUtils.close(dialogue)
            }
        }
    }

    Component {
        id: disablesInternetWarning
        Dialog {
            id: dialogue
            property OfonoContextConnection mms
            property OfonoContextConnection combined
            /* TRANSLATORS: %1 is the MMS APN that the user has chosen to be
            “preferred”. */
            title: i18n.tr("Prefer %1").arg(mms.name)
            /* TRANSLATORS: %1 is the MMS APN that the user has chosen to be
            “preferred”, i.e. used to retrieve MMS messages. %2 is the Internet
            APN that will be “de-preferred” as a result of this action. */
            text: i18n.tr("You have chosen %1 as your preferred MMS APN. " +
                          "This disconnects an Internet connection provided " +
                          "by %2.").arg(mms.name).arg(combined.name)

            Button {
                text: i18n.tr("Disconnect")
                onClicked: {
                    Manager.setPreferred(mms, true, true);
                    PopupUtils.close(dialogue);
                }
            }

            Button {
                text: i18n.tr("Cancel")
                onClicked: PopupUtils.close(dialogue)
            }
        }
    }

    Component {
        id: disablesMMSWarning
        Dialog {
            id: dialogue
            property OfonoContextConnection internet
            property OfonoContextConnection combined
            /* TRANSLATORS: %1 is the Internet APN that the user has chosen to
            be “preferred”. */
            title: i18n.tr("Prefer %1").arg(internet.name)
            /* TRANSLATORS: %1 is the Internet APN that the user has chosen to
            be “preferred”, i.e. used to connect to the Internet. %2 is the MMS
            APN that will be “de-preferred” as a result of this action. */
            text: i18n.tr("You have chosen %1 as your preferred Internet APN. " +
                          "This disables the MMS configuration provided " +
                          "by %2.").arg(internet.name).arg(combined.name)

            Button {
                text: i18n.tr("Disable")
                onClicked: {
                    Manager.setPreferred(internet, true, true);
                    PopupUtils.close(dialogue);
                }
            }

            Button {
                text: i18n.tr("Cancel")
                onClicked: PopupUtils.close(dialogue)
            }
        }
    }

    Component {
        id: disableContextWarning
        Dialog {
            id: dialogue
            property OfonoContextConnection context
            /* TRANSLATORS: %1 is the APN that the user has disconnected or
            disabled. */
            title: context.type === 'internet' ?
                i18n.tr("Disconnect %1").arg(context.name) :
                i18n.tr("Disable %1").arg(context.name)

            /* TRANSLATORS: %1 is the APN that the user has disconnected or
            disabled. */
            text: context.type === 'internet' ?
                i18n.tr("This disconnects %1.").arg(context.name) :
                i18n.tr("This disables %1.").arg(context.name)

            Button {
                text: context.type === 'mms' ?
                    i18n.tr("Disable") :
                    i18n.tr("Disconnect")
                onClicked: {
                    Manager.setPreferred(context, false, true);
                    PopupUtils.close(dialogue);
                }
            }

            Button {
                text: i18n.tr("Cancel")
                onClicked: PopupUtils.close(dialogue)
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

        ListItem.Standard {
            id: apnListItem
            property alias text: apnItemName.text
            objectName: "edit_" + qml.name
            height: units.gu(6)
            removable: true
            confirmRemoval: true
            progression: true

            onItemRemoved: Manager.removeContext(path);
            onClicked: {
                editor = pageStack.push(pageApnEditor, {
                    contextQML:      qml,
                    mmsModel:        mmsContexts,
                    internetModel:   internetContexts,
                    iaModel:         iaContexts
                });
            }

            control: CheckBox {
                id: check
                objectName: qml.name + "_preferred"
                property bool serverChecked: qml && qml.preferred
                onServerCheckedChanged: checked = serverChecked
                Component.onCompleted: checked = serverChecked
                onTriggered: Manager.setPreferred.call(this, qml, checked)
            }

            Item  {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    leftMargin: units.gu(2)
                    right: parent.right
                }

                Label {
                    id: apnItemName
                    anchors {
                        topMargin: units.gu(1)
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

        onContextAdded: Manager.contextAdded(path)
        onContextRemoved: Manager.contextRemoved(path)
        onContextsChanged: Manager.contextsChanged(contexts)
        onReportError: Manager.reportError(message)
        Component.onCompleted: Manager.contextsChanged(sim.connMan.contexts)
    }

    // We set the target to be ConnMan before we want to call 'ResetContexts' on
    // ConnMan. When ConnMan powers down, the connManPoweredChanged handler is
    // called and call 'ResetContexts'. This is because we can't make this call
    // while ConnMan is 'Powered'. After the 'ResetContexts' call is done,
    // the target is reset to null.
    Connections {
        id: restorePowered
        target: null
        ignoreUnknownSignals: true
        onPoweredChanged: Manager.connManPoweredChanged(powered)
    }
}
