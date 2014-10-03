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

ItemPage {
    id: root
    title: i18n.tr("APN")
    objectName: "apnPage"

    property var sim

    QtObject {
        id: d

        // map of contextPath : connCtx
        property var mContexts: ({})

        readonly property string mCustomContextNameInternet: "___ubuntu_custom_apn_internet"
        readonly property string mCustomContextNameMms: "___ubuntu_custom_apn_mms"
        property var mCustomContextInternet : undefined
        property var mCustomContextMms : undefined

        // LP(:#1362795)
        property var pendingCustomMmsData : ({})

        // suppress any actions that we don't want to take
        // when updating selectedIndexes, etc
        property bool __suppressActivation : true;

        function isEmptyCustom (type, ctx)
        {
            /* OK, this sucks _hard_,
             * QOfono does not return the added context, so instead we have to "figure it out"
             * by looking for "contextAdded" for totally empty context with default Name values.
             * LP(#1361864)
             */

            var targetName = "";
            var targetAccessPointName = "";
            if (type === "internet") {
                targetName = "Internet";
                targetAccessPointName = "";
            } else if (type == "mms") {
                targetName = "MMS";
                targetAccessPointName = "";
            }

            if (ctx.type === type &&
                ctx.name === targetName &&
                ctx.accessPointName === targetAccessPointName)
                return true;
            return false;
        }

        function updateContexts()
        {
            var tmp = sim.connMan.contexts.slice(0);
            var added = tmp.filter(function(i) {
                return mContexts[i] === undefined;
            });
            var removed = Object.keys(mContexts).filter(function(i) {
                return tmp.indexOf(i) === -1;
            })

            removed.forEach(function(currentValue, index, array) {
                // just asserting to verify the logic
                // remove once proven functional
                if (mContexts[currentValue] === undefined) {
                    throw "updateContexts: removed is broken";
                }

                if (mContexts[currentValue].name === mCustomContextNameInternet)
                    mCustomContextInternet = undefined
                else if (mContexts[currentValue].name === mCustomContextNameMms)
                    mCustomContextMms = undefined

                mContexts[currentValue].destroy();
                delete mContexts[currentValue];
            });

            added.forEach(function(currentValue, index, array) {
                // just asserting to verify the logic
                // remove once proven functional
                if (mContexts[currentValue] !== undefined) {
                    throw "updateContexts: added is broken";
                }

                var ctx = connCtx.createObject(parent,
                                               {
                                                   "contextPath": currentValue
                                               });
                mContexts[currentValue] = ctx;

                if (isEmptyCustom("internet", ctx))
                {
                    ctx.name = mCustomContextNameInternet;
                    // name updates async, so return here and
                    // have the buildLists() called from Context::onNameChanged
                    return;
                } else if (isEmptyCustom("mms", ctx))
                {
                    ctx.name = mCustomContextNameMms;
                    ctx.accessPointName = pendingCustomMmsData["accessPointName"];
                    ctx.username = pendingCustomMmsData["username"];
                    ctx.password = pendingCustomMmsData["password"];
                    ctx.messageCenter = pendingCustomMmsData["messageCenter"];
                    ctx.messageProxy = pendingCustomMmsData["messageProxy"];
                    pendingCustomMmsData = ({});
                    // values update async, so return here and
                    // have the buildLists() called from Context::onNameChanged
                    return;
                }
            });

            // just asserting to verify the logic
            // remove once proven functional
            if (Object.keys(mContexts).length !== tmp.length) {
                throw "Object.keys(contexts).length !== tmp.length";
            }
            tmp.forEach(function(currentValue, index, array) {
                if (mContexts[currentValue] === undefined)
                    throw "contexts[currentValue] === undefined";
            });

            buildLists();
        }

        // expects updateContexts() to have ran before executing.
        function checkAndCreateCustomContexts()
        {
            var customInternet = Object.keys(mContexts).filter(function (i) {
                var ctx = mContexts[i];
                return ctx.name === mCustomContextNameInternet ||
                       isEmptyCustom("internet", ctx);
            });
            var customMms = Object.keys(mContexts).filter(function (i) {
                var ctx = mContexts[i];
                return ctx.name === mCustomContextNameMms ||
                       isEmptyCustom("mms", ctx);
            });

            // make sure there is only one context per type
            if (customInternet.length > 1) {
                customInternet.forEach(function(currentValue, index, array) {
                   if (index === 0)
                       return;
                   sim.connMan.removeContext(currentValue);
                });
            }
            if (customMms.length > 1) {
                customMms.forEach(function(currentValue, index, array) {
                   if (index === 0)
                       return;
                   sim.connMan.removeContext(currentValue);
                });
            }

            if (customInternet.length === 0) {
                sim.connMan.addContext("internet");
            }

            // @bug don't create the custom MMS context
            //      LP(:#1362795)
            // if (customMms.length === 0) {
            //     sim.connMan.addContext("mms");
            // }

            buildLists();
        }

        property var mInternetApns : [];
        property var mMmsApns : [];
        function buildLists()
        {
            d.__suppressActivation = true;

            var internet = [];
            var mms = [];

            internet = Object.keys(mContexts).filter(function(i) {
                var ctx = mContexts[i];
                if (ctx.type === "internet") {
                    if (ctx.name === mCustomContextNameInternet) {
                        mCustomContextInternet = ctx
                        // don't add yet
                        return false;
                    }
                    return true;
                }
                return false;
            });
            mms = Object.keys(mContexts).filter(function(i) {
                var ctx = mContexts[i];
                if ( ctx.type === "mms") {
                    if (ctx.name === mCustomContextNameMms) {
                        mCustomContextMms = ctx;
                        // don't add yet
                        return false;
                    }
                    return true;
                }
                return false;
            });

            // make sure customized are the last on the lists
            if (mCustomContextInternet !== undefined)
                internet = internet.concat([mCustomContextInternet.contextPath])
            if (mCustomContextMms !== undefined)
                mms = mms.concat([mCustomContextMms.contextPath])
            else {
                /// @bug LP(#1361864)
                // add anyway a "dummy" Custom so we can show at least the one provisioned
                // MMS context as long as the user does not hit "Custom" in the MMS list.
                mms = mms.concat(["dummycustom"])
            }

            // add "Same APN as for Internet" to be the first on the MMS list
            mms = ["/same/as/internet"].concat(mms);

            mInternetApns = internet;
            mMmsApns = mms;

            d.__suppressActivation = false;
        }

        function openApnEditor(type) {
            var ctx;
            if (type === "internet") {
                ctx = mCustomContextInternet;
            } else if (type == "mms") {
                ctx = mCustomContextMms;
            }
            /// can't modify active context
            if (ctx !== undefined && ctx.active)
                ctx.active = false;

            pageStack.push(Qt.resolvedUrl("CustomApnEditor.qml"),
                           {
                               type: type,
                               contexts: {"internet": mCustomContextInternet,
                                          "mms":      mCustomContextMms},
                               activateCb: activateHelper
                           });
        }

        function activateHelper(type, contextPath, customMmsData) {
            if (type === "internet")
                activator.activate(contextPath, sim.simMng.subscriberIdentity, sim.simMng.modemPath)
            if (type === "mms") {
                if (contextPath === undefined) {
                    // LP(:#1362795)
                    pendingCustomMmsData = customMmsData
                    /// remove any provisioned ones..
                    var remove = []
                    Object.keys(mContexts).forEach(function(currentValue, index, array) {
                        var ctx = mContexts[currentValue];
                        if (ctx.type === "mms")
                            remove = remove.concat([ctx.contextPath])
                    });
                    remove.forEach(function(currentValue, index, array) {
                        sim.connMan.removeContext(currentValue);
                    });
                    sim.connMan.addContext("mms")
                }
            }
        }
    }

    OfonoActivator {
        id:activator
    }

    Component {
        id: connCtx
        OfonoContextConnection {

            // add helper property to detect dual internet/MMS contexts
            property bool dual : false
            Component.onCompleted:{
                if (type == "internet")
                    if (messageCenter !== "")
                        dual = true
            }

            onActiveChanged: if (type === "internet") internetApnSelector.updateSelectedIndex()
            onNameChanged: d.buildLists()
            onAccessPointNameChanged: d.buildLists()
            onReportError: console.error("Context error on " + contextPath + ": " + errorString)
        }
    }

    Connections {
        target: sim.connMan
        onContextsChanged: d.updateContexts()

        Component.onCompleted: {
            d.updateContexts();
            // do this once.
            d.checkAndCreateCustomContexts();
        }
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick

        Column {

            anchors {
                left: parent.left
                right: parent.right
            }

            ListItem.Standard {
                id: heading1
                objectName: "internetapn"
                text: i18n.tr("Internet APN:")
                progression: false
            }
            ListItem.ThinDivider {}
            ListItem.SingleControl {
                control: ListItem.ItemSelector {
                    id: internetApnSelector
                    width: parent.width - units.gu(4)
                    model: d.mInternetApns
                    expanded: true
                    selectedIndex: -1
                    onModelChanged: updateSelectedIndex()

                    function updateSelectedIndex()
                    {
                        var tmp = d.__suppressActivation
                        d.__suppressActivation = true;
                        var idx = -1;
                        if (model) {
                            model.forEach(function(currentValue, index, array) {
                                if (d.mContexts[currentValue].active)
                                    idx = index;
                            });
                        }
                        selectedIndex = idx;
                        d.__suppressActivation = tmp;
                    }

                    delegate: OptionSelectorDelegate {
                        showDivider: false
                        text: {
                            var ctx = d.mContexts[modelData];
                            if (ctx.name !== "") {
                                if (ctx.name !== d.mCustomContextNameInternet) {
                                    return ctx.name
                                } else {
                                    //: user visible name of the custom Internet APN
                                    return i18n.tr("Custom");
                                }
                            } else {
                                return ctx.accessPointName
                            }
                        }
                    }
                    onSelectedIndexChanged: {
                        if (selectedIndex === -1) {
                            if (mmsApnSelector && mmsApnSelector.model[mmsApnSelector.selectedIndex] === "/same/as/internet")
                                mmsApnSelector.selectedIndex = -1;
                            return;
                        }

                        var ctx = d.mContexts[model[selectedIndex]];
                        if(ctx.dual) {
                            if (!d.mCustomContextMms)
                                mmsApnSelector.selectedIndex = mmsApnSelector.model.indexOf("/same/as/internet");
                        }
                        else if (mmsApnSelector.model[mmsApnSelector.selectedIndex] === "/same/as/internet")
                            mmsApnSelector.selectedIndex = -1;

                        if (d.__suppressActivation)
                            return;

                        if (d.mCustomContextInternet && model[selectedIndex] === d.mCustomContextInternet.contextPath) {
                            if (d.mCustomContextInternet.accessPointName === "") {
                                d.openApnEditor("internet");
                                updateSelectedIndex();
                                return;
                            }
                        }

                        d.activateHelper(ctx.contextPath);
                    }
                }
            }
            ListItem.SingleControl {
                control: Button {
                    objectName: "customApnEdit"
                    text: i18n.tr("Custom Internet APN…")
                    width: parent.width - units.gu(4)
                    onClicked: d.openApnEditor("internet")
                }
            }

            ListItem.Divider {}

            ListItem.Standard {
                id: heading2
                objectName: "mmsapn"
                text: i18n.tr("MMS APN:")
                progression: false
            }
            ListItem.ThinDivider {}
            ListItem.SingleControl {
                control: ListItem.ItemSelector {
                    id: mmsApnSelector
                    width: parent.width - units.gu(4)
                    model: d.mMmsApns
                    expanded: true
                    selectedIndex: -1
                    delegate: OptionSelectorDelegate {
                        showDivider: modelData === "/same/as/internet"
                        enabled: {
                            if (modelData !== "/same/as/internet")
                                return true;
                            else
                                d.mContexts[internetApnSelector.model[internetApnSelector.selectedIndex]].dual
                        }
                        // work around OptionSelectorDelegate not having a visual change depending on being disabled
                        opacity: enabled ? 1.0 : 0.5
                        text: {
                            if (modelData === "/same/as/internet") {
                                return i18n.tr("Same APN as for Internet");
                            }
                            if (modelData === "dummycustom") {
                                return i18n.tr("Custom");
                            }
                            var ctx = d.mContexts[modelData];
                            if (ctx.name !== "") {
                                if (ctx.name !== d.mCustomContextNameMms) {
                                    return ctx.name
                                } else {
                                    //: user visible name of the custom MMS APN
                                    return i18n.tr("Custom");
                                }
                            } else {
                                return ctx.accessPointName
                            }
                        }
                    }
                    onModelChanged: updateSelectedIndex();
                    function updateSelectedIndex()
                    {
                        // if we have custom MMS context, it must be active.
                        // @bug LP(#1361864)
                        var tmp = d.__suppressActivation;
                        d.__suppressActivation = true;
                        if (d.mCustomContextMms) {
                            selectedIndex = model.indexOf(d.mCustomContextMms.contextPath);
                        } else if (model.length === 3) {
                            /* meaning we have:
                             * 0 - /same/as/internet
                             * 1 - some provisioned one
                             * 2 - dummycustom
                             */
                            selectedIndex = 1;
                        } else if (internetApnSelector.model && internetApnSelector.selectedIndex !== -1) {
                            if (d.mContexts[internetApnSelector.model[internetApnSelector.selectedIndex]].dual) {
                                selectedIndex = model.indexOf("/same/as/internet");
                            } else
                                selectedIndex = -1;
                        } else {
                            selectedIndex = -1;
                        }
                        d.__suppressActivation = tmp;
                    }

                    onSelectedIndexChanged: {
                        if (selectedIndex === -1 || d.__suppressActivation)
                            return;

                        if (model[selectedIndex] === "/same/as/internet") {
                            // @bug delete _any_ actual MMS context
                            //      LP:(#1362795)
                            var remove = [];
                            Object.keys(d.mContexts).forEach(function(currentValue, index, array) {
                                var ctx = d.mContexts[currentValue];
                                if (ctx.type === "mms")
                                    remove = remove.concat([ctx.contextPath]);
                            });
                            remove.forEach(function(currentValue, index, array) {
                                sim.connMan.removeContext(currentValue);
                            });
                            return;
                        }

                        if (model[selectedIndex] === "dummycustom") {
                            d.openApnEditor("mms");
                            updateSelectedIndex()
                            return;
                        }

                        // no need to "activate" anything.
                        // just fall through return here.
                        // once we actually are able to suppport multiple MMS contexts
                        // on the system, add some code here to set one of them active
                    }
                }
            }
            ListItem.SingleControl {
                control: Button {
                    objectName: "customApnEdit"
                    text: i18n.tr("Custom MMS APN…")
                    width: parent.width - units.gu(4)
                    onClicked: d.openApnEditor("mms")
                }
            }

            // @todo: no means of doing any meaningful reset right now.
            // LP(#1338758)
            // ListItem.ThinDivider {}
            // ListItem.SingleControl {
            //     control: Button {
            //         objectName: "resetButton"
            //         text: i18n.tr("Reset APN Settings")
            //         width: parent.width - units.gu(4)
            //         onClicked: {
            //             PopupUtils.open(resetDialog)
            //         }
            //     }
            // }
        }
    }

    Component {
         id: resetDialog
         Dialog {
             id: dialogue
             title: i18n.tr("Reset APN Settings")
             text: i18n.tr("Are you sure that you want to Reset APN Settings?")
             Button {
                 text: i18n.tr("Cancel")
                 onClicked: PopupUtils.close(dialogue)
             }
             Button {
                 text: i18n.tr("Reset")
                 color: UbuntuColors.orange
                 onClicked: {
                     // delete all APNs
                     // kick ofono per
                     // https://bugs.launchpad.net/ubuntu/+source/ofono/+bug/1338758

                 }
             }

         }
    }
}
