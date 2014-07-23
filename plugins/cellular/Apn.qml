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
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import MeeGo.QOfono 0.2

ItemPage {
    id: root
    title: i18n.tr("APN")
    objectName: "apnPage"

    property var connMan
    // arrays holding the APN contexts
    property variant internetApns: []
    property variant mmsApns: []
    property variant iapnNames: []
    property variant mapnNames: []
    // pointers tot the active contexts, note none may be active
    property int iActive: -1
    property int mActive: -1
    property int iCustom: -1
    property int mCustom: -1

    Connections {
        target: connMan
        onContextsChanged: buildLists();
        onContextRemoved: console.warn("Removed context: " + path)
        Component.onCompleted: {
            // todo can this be optimized
            buildLists();
        }
    }

    function buildLists()
     {
        var iNames = new Array();
        var mNames = new Array();
        var iApns = new Array();
        var mApns = new Array();

        console.warn("Found " + connMan.contexts.length + " contexts")
        for (var i = 0; i < connMan.contexts.length; i++)
        {
            var newApn = apnContext.createObject(parent, {"contextPath": connMan.contexts[i]});
            console.warn("Path: " + connMan.contexts[i] + " Name: " + newApn.name + " Type: " + newApn.type);
            if (newApn.type.indexOf("internet") !== -1 ) {
                iApns.push(newApn);
                if (newApn.active) {
                    iActive = iNames.length;
                    activeContexts.internet = newApn;
                }
                if (newApn.name === "custom") {
                    iCustom = iNames.length;
                    iNames.push(newApn.accessPointName);
                    activeContexts.custominternet = newApn;
                } else
                    iNames.push(newApn.name);

            }
            if (newApn.type.indexOf("mms") !== -1 || newApn.messageCenter !== "") {
                mApns.push(newApn);
                if (newApn.active) {
                    mActive = mNames.length;
                    activeContexts.mms = newApn;
                }
                if (newApn.name === "custom") {
                    mCustom = mNames.length;
                    mNames.push(newApn.accessPointName);
                    activeContexts.custommms = newApn;
                } else
                    mNames.push(newApn.name);
            }
        }

        internetApns = iApns;
        iapnNames = iNames;
        mmsApns = mApns;
        mapnNames = mNames;
        console.warn("active is " + iActive)
    }

    function customAction(type) {
        pageStack.push(Qt.resolvedUrl("CustomApn.qml"), {connMan: connMan, type: type, activeContexts: activeContexts})
    }

    Component {
        id: apnContext
        OfonoContextConnection {
            onActiveChanged: {
                console.warn("Connection changed ")
                // can we get the reporting object here? otherwise rebuild the lists
            }
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

    Item {
        id: activeContexts
        property variant internet
        property variant mms
        property variant custominternet
        property variant custommms
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
            Repeater {
                model: iapnNames
                ListItem.Standard {
                    progression: true
                    text: modelData
                    selected: index === iActive
                    showDivider: false
                    onClicked:
                        pageStack.push(Qt.resolvedUrl("ViewApn.qml"), {connMan: connMan, theContext: internetApns[index]})
                }
            }
            ListItem.Standard {
                id: customInternet
                objectName: "customInternetItem"
                text: i18n.tr("Custom")
                progression: true
                onClicked: customAction("internet")
            }

            ListItem.ThinDivider {}

            ListItem.Standard {
                id: heading2
                objectName: "mmsapn"
                text: i18n.tr("MMS APN:")
                progression: false
            }
            ListItem.ThinDivider {}
            Repeater {
                model: mapnNames
                ListItem.Standard {
                    progression: true
                    text: modelData
                    selected: index === mActive
                    showDivider: false
                    onClicked:
                        pageStack.push(Qt.resolvedUrl("ViewApn.qml"), {connMan: connMan, theContext: mmsApns[index]})
                }
            }
            ListItem.Standard {
                id: customMms
                objectName: "customMmsItem"
                text: i18n.tr("Custom")
                progression: true
                onClicked: customAction("mms")
            }
            ListItem.ThinDivider {}

            ListItem.SingleControl {
                control: Button {
                    objectName: "resetButton"
                    text: i18n.tr("Reset APN Settings")
                    width: parent.width - units.gu(4)
                    onClicked: {
                        PopupUtils.open(resetDialog)
                    }
                }
            }
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
