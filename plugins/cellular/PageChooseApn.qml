/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Pat McGowan <pat.mcgowan@canonical.com>
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
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import MeeGo.QOfono 0.2
import Ubuntu.SystemSettings.Cellular 1.0
import "apn.js" as APN

ItemPage {
    id: root
    title: i18n.tr("APN")
    objectName: "apnPage"

    property var sim

     /* Provide functionality to manage editor */
    QtObject {
        id: d

        // The instantiated editor component (a Dialog)
        property var editor

        /* A cache for the values entered by the user.
        The creation of a context is async, so when it is created,
        we use these values to update the context */
        property var editorValues

        /**
         * Closes the editor
         */
        function closeEditor () {
            PopupUtils.close(d.editor);
            d.editor = null;
            d.editorValues = null;
        }

        /**
         * Open the editor
         *
         * @param {String} type of editor we're about to edit
         * @param {QOfonoConnectionContext|Null} apn we're editing, null if new
         * @param {QOfonoConnectionContext|Null} activeInternetApn
         *     used when editing mms as apn to draw values from
         */
        function openEditor (apn, type, activeInternetApn) {
            var data = {
                'type': type,
                'apn': apn
            };
            d.editor = PopupUtils.open(editor, root, data);
        }

        property string customInternetString: i18n.tr("Custom")
        property string customMMSString: i18n.tr("Custom")
    }

    OfonoActivator {
        id: activator
    }

    /* These are the models that will hold */
    ListModel {
        id: internetContexts
    }

    ListModel {
        id: lteContexts
    }

    ListModel {
        id: mmsContexts
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(2)

            /* This repeater repeats once for internet and once for mms.
                modelData will be either 'internet' or 'mms'
            */
            Repeater {
                id: repeater
                anchors { left: parent.left; right: parent.right }
                height: childrenRect.height

                model: ['internet', 'mms']

                Column {

                    /*
                    @param {String|null} contextPath that was activated, if any
                    */
                    function refreshSelectedIndex (contextPath, contextState) {
                        console.warn('refreshSelectedIndex', modelData);
                        contextSelector.selectedIndex = APN.getSelectedIndex(
                            contextSelector.model, contextPath, contextState);

                    }

                    anchors { left: parent.left; right: parent.right }
                    spacing: units.gu(2)

                    ListItem.ItemSelector {
                        id: contextSelector
                        text: modelData === 'internet' ?
                            i18n.tr("Internet APN:") : i18n.tr("MMS APN:")
                        model: modelData === 'internet' ? netCtxModel : mmsCtxModel
                        visible: model.count
                        expanded: true
                        delegate: OptionSelectorDelegate {
                            text: APN.getFriendlyName(model)
                            showDivider: false
                        }
                        onModelChanged: refreshSelectedIndex()
                        onDelegateClicked: APN.activateContext(model.get(index))
                    }

                    // Selector for no contexts
                    ListItem.ItemSelector {
                        text: contextSelector.text;
                        model: 1;
                        expanded: true;
                        visible: !contextSelector.visible
                        selectedIndex: -1
                        delegate: OptionSelectorDelegate {
                            text: i18n.tr("None")
                            enabled: false
                            showDivider: false
                        }
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: parent.width - units.gu(4)
                        text: modelData === 'internet' ?
                            i18n.tr("Custom Internet APN…") : i18n.tr("Custom MMS APN…")
                        onTriggered: {

                            var ctx;

                            /* before we present the user a custom context,
                            delete any duplicate, custom contexts */
                            var duplicatesRemoved = APN.removeDuplicateContexts(
                                sim.connMan.contexts, sim.path);

                            if (modelData === 'internet') {
                                ctx = APN.getCustomInternetContext(duplicatesRemoved, sim.path);
                            } else {
                                ctx = APN.getCustomMMSContext(duplicatesRemoved, sim.path);
                            }

                            /* If mms, provide the editor with the current
                            internet context, if any */
                            var activeInternetContext;

                            if (modelData === 'mms') {
                                activeInternetContext = APN.getActiveInternetContext(
                                    sim.connMan.contexts, sim.path);
                            }
                            d.openEditor(ctx, modelData, activeInternetContext);
                        }
                    }
                }
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
        id: contextComponent

        OfonoContextConnection {
            id: ctx

            // add helper property to detect dual internet/MMS contexts
            property bool dual : false

            onTypeChanged: {

                // set dual property
                if (type === 'internet') {
                    if (messageCenter !== '') {
                        dual = true;
                        // slap models
                        sim.connMan.contextsChanged(sim.connMan.contexts);
                    }
                }

                console.warn('onTypeChanged, ctx.type', ctx.type, 'type:', type);
                if (type === 'internet') {
                    netCtxModel.append(ctx);
                } else if (type === 'mms') {
                    mmsCtxModel.append(ctx);
                }
            }

            onActiveChanged: {
                var i;
                console.warn('Active changed for', ctx.contextPath, active);
                for (i = 0; i < repeater.count; i++) {
                    repeater.itemAt(i).refreshSelectedIndex(ctx.contextPath, active);
                }
            }
            onDisconnectRequested: {
                console.warn('Disconnect requested on', ctx.contextPath);
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

            onSaved: {
                d.editorValues = values;

                if (apn) {
                    // existing apn
                    console.warn('Saved existing context', d.editorValues);
                    APN.updateContext(apn, d.editorValues);
                    PopupUtils.close(d.editor);

                } else {
                    /* This adds a new context, which we try to detect
                    in connMan.contextAdded. Note that addContext does not return
                    anything. */
                    console.warn('Adding new context');
                    sim.connMan.addContext(values.type);
                    editorDialog.activating();
                }
            }

            onCanceled: {
                d.closeEditor();
            }
        }
    }

    Connections {
        target: sim.connMan

        onReportError: {
            console.warn(message);
            if (d.editor) {
                editor.failed(message);
            }
        }

        onContextRemoved: APN.deleteQML(path)

        /*
        Determine if we want to edit the new context in two steps:
            1. Check if the editor is open
            2. Create OfonoConnectionContext for context, and wait for
                the type property to be populated. If it's the same type,
                _assume_ it's the one we want.
        */
        onContextAdded: {
            console.warn('onContextAdded')
            // This is a bit subtle, but this creates QML for newly added contexts
            var context  = APN.getOrCreateContextQML(path, sim.path);

            /* When we add a context, nothing is immediately returned from ofono,
            so this is a workaround: we wait for the type to change on the associated
            QML object, and then see if the type matches whatever's in the editor. */
            function _typeChanged(type) {
                if (type === d.editorValues['type']) {
                    console.warn('Context was added and is most def the one we asked for', context.name);
                    APN.updateContext(context, d.editorValues);
                    if (!context.active) {
                        APN.activateContext(context);
                    }
                    context.typeChanged.disconnect(_typeChanged);
                }
            }

            /* The context newly created was added. Wait for the type
            on the context to change before updating it with values from the
            editor */
            if (d.editor) {
                console.warn('Context was added while editor is open.')
                context.typeChanged.connect(_typeChanged);
            }
        }

        onContextsChanged: {
            console.warn('onContextsChanged')
            APN.updateQML(contexts)
        }

        Component.onCompleted: APN.updateQML(sim.connMan.contexts)
    }
}
