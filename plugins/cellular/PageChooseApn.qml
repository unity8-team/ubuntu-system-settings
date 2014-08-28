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
    // arrays holding the APN contexts


    QtObject {
        id: d

        // map of contextPath : connCtx
        property var mContexts: ({})

        readonly property string mCustomContextNameInternet: "___ubuntu_custom_apn_internet"
        readonly property string mCustomContextNameMms: "___ubuntu_custom_apn_mms"
        property var mCustomContextInternet : undefined
        property var mCustomContextMms : undefined

        // work around OptionSelector index behaviour
        // LP(#1361915)
        property bool __suppressActivation : true;

        function updateContexts()
        {
            var tmp = sim.connMan.contexts;
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

                /* OK, this sucks _hard_,
                 * QOfono does not return the added context, so instead we have to "figure it out"
                 * by looking for "contextAdded" for totally empty context with default Name values.
                 * LP(#1361864)
                 */
                if (ctx.type === "internet" &&
                    ctx.name === "Internet" &&
                    ctx.accessPointName === "")
                {
                    ctx.name = mCustomContextNameInternet;
                    return;
                } else if (ctx.type === "mms" &&
                          ctx.name === "MMS" &&
                          ctx.accessPointName === "")
                {
                    ctx.name = mCustomContextNameMms;
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

            console.warn("Found " + Object.keys(mContexts).length + " contexts")
            buildLists();
        }

        // expects updateContexts() to have ran before executing.
        function checkAndCreateCustomContexts()
        {
            var customInternet = Object.keys(mContexts).filter(function (i) {
                return mContexts[i].name === mCustomContextNameInternet;
            });
            var customMms = Object.keys(mContexts).filter(function (i) {
                return mContexts[i].name === mCustomContextNameMms;
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
            if (customMms.length === 0) {
                sim.connMan.addContext("mms");
            }

            buildLists();
        }

        property var mInternetApns : [];
        property var mMmsApns : [];
        function buildLists()
        {
            __suppressActivation = true;

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
            if (ctx.active)
                ctx.active = false;

            pageStack.push(Qt.resolvedUrl("CustomApnEditor.qml"),
                           {
                               type: type,
                               contexts: {"internet": mCustomContextInternet,
                                          "mms":      mCustomContextMms},
                               activateCb: activateHelper
                           });
        }

        function activateHelper(contextPath) {
            activator.activate(contextPath, sim.simMng.subscriberIdentity, sim.simMng.modemPath)
        }
    }

    OfonoActivator {
        id:activator
    }

    Component {
        id: connCtx
        OfonoContextConnection {

            property bool dual : false
            Component.onCompleted:{
                if (type == "internet")
                    if (messageCenter !== "")
                        dual = true
            }

            onActiveChanged: {
                d.__suppressActivation = true;
                if (this.active) {
                    if (this.type == "internet") {
                        internetApnSelector.selectedIndex = d.mInternetApns.indexOf(this.contextPath);
                    } else if (this.type == "mms") {
                        mmSApnSelector.selectedIndex = d.mMmsApns.indexOf(this.contextPath);
                    }
                }
                d.__suppressActivation = false;
            }
            onNameChanged: d.buildLists()
            onAccessPointNameChanged: d.buildLists()

            onProvisioningFinished: {
                console.warn("Provisioned")
            }
            onReportError: {
                console.warn("Context error: " + errorString)
                // how to know which list to set unselected?
                // todo pop up an error dialog
            }
        }
    }

    Connections {
        target: sim.connMan
        onContextsChanged: {
            d.updateContexts()
        }

        Component.onCompleted: {
            // do this once.
            d.updateContexts();
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
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Standard {
                id: heading1
                objectName: "internetapn"
                text: i18n.tr("Internet APN:")
                progression: false
            }
            ListItem.ThinDivider {}
            ListItem.SingleControl {
                control: OptionSelector {
                    id: internetApnSelector
                    width: parent.width - units.gu(4)
                    model: d.mInternetApns
                    expanded: true
                    delegate: OptionSelectorDelegate {
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
                    onModelChanged: selectedIndex = -1;
                    onSelectedIndexChanged: {
                        if (selectedIndex === -1) {
                            if (mmsApnSelector.model[mmsApnSelector.selectedIndex] === "/same/as/internet")
                                                        mmsApnSelector.selectedIndex = -1
                            return;
                        }

                        var ctx = d.mContexts[model[selectedIndex]];
                        if(ctx.dual)
                            mmsApnSelector.selectedIndex = mmsApnSelector.model.indexOf("/same/as/internet")
                        else if (mmsApnSelector.model[mmsApnSelector.selectedIndex] === "/same/as/internet")
                            mmsApnSelector.selectedIndex = -1

                        if (d.__suppressActivation)
                            return;

                        console.warn(ctx.contextPath)
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

            ListItem.ThinDivider {}

            ListItem.Standard {
                id: heading2
                objectName: "mmsapn"
                text: i18n.tr("MMS APN:")
                progression: false
            }
            ListItem.ThinDivider {}
            ListItem.SingleControl {
                control: OptionSelector {
                    id: mmsApnSelector
                    width: parent.width - units.gu(4)
                    model: d.mMmsApns
                    expanded: true
                    delegate: OptionSelectorDelegate {
                        showDivider: modelData === "/same/as/internet"
                        text: {
                            if (modelData === "/same/as/internet") {
                                return i18n.tr("Same APN as for Internet");
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
                    onModelChanged: selectedIndex = -1;
                    onSelectedIndexChanged: {
                        if (selectedIndex === -1 || d.__suppressActivation)
                            return;

                        if (model[selectedIndex] === "/same/as/internet") {
                            // deactivate any separate MMS context
                            Object.keys(d.mContexts).forEach(function(currentValue, index, array) {
                                var ctx = d.mContexts[currentValue];
                                if (ctx.type === "mms" && ctx.active)
                                    ctx.deactivate()
                            });
                            return;
                        }

                        var ctx = d.mContexts[model[selectedIndex]];
                        console.warn(ctx.contextPath)
                        d.activateHelper(ctx.contextPath);
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
//            ListItem.ThinDivider {}
//            ListItem.SingleControl {
//                control: Button {
//                    objectName: "resetButton"
//                    text: i18n.tr("Reset APN Settings")
//                    width: parent.width - units.gu(4)
//                    onClicked: {
//                        PopupUtils.open(resetDialog)
//                    }
//                }
//            }

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
