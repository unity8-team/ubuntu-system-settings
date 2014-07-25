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
import QtQuick.Layouts 1.1
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import MeeGo.QOfono 0.2
import Ubuntu.SystemSettings.Cellular 1.0

ItemPage {
    objectName: "customapnPage"
    id: root

    property var type
    property variant connMan
    property variant activeContext
    property var activeContexts
    property var typeText
    property var ismms

    OfonoActivator {
        id:activator
    }

    typeText: type === "internet" ? i18n.tr("Internet") : i18n.tr("MMS")
    ismms: type === "mms"
    title: i18n.tr("Custom ") + typeText + i18n.tr(" APN")

    flickable: scrollWidget

    function validateFields() {
        // todo
        return true;
    }

    function createApn (path) {
        var customContext
        customContext = apnContext.createObject(parent, {"contextPath": path} );
        console.warn("Made a context object for  + path")
        return customContext;
    }

    function initialize(theContext) {

        console.warn("setting fields")
        // prepopulate the fields
        apnName.text = theContext.accessPointName;
        userName.text = theContext.username;
        pword.text = theContext.password;
        // todo map protocol values
        //protocol.selectedIndex = map(theContext.protocol);
        if (ismms) {
            mmsc.text = theContext.messageCenter;
            var proxyText = theContext.messageProxy.split(":")
            proxy.text = proxyText[0];
            port.text = proxyText[1];
        }

    }

    function updateContext(theContext) {

        if (!validateFields()) return;
        console.warn("updating");

        theContext.name = "custom";
        // type will be set already theContext.type = type;
        theContext.accessPointName = apnName.text;
        theContext.username = userName.text;
        theContext.password = pword.text;
        //theContext.protocol = map(protocol.text);
        if (ismms) {
            theContext.messageCenter = mmsc.text;
            // assume this is how to add the port
            port.length == 0 ? theContext.messageProxy = proxy.text : theContext.messageProxy = proxy.text + ":" + port.text;
        }
        // assume we want the new one active?
        //theContext.active = true;
        //activator.activate(theContext.contextPath)
        console.warn("APN updated: " + theContext.name)
    }

    function createContext () {
        connMan.addContext(type);
        console.warn("called add context for type " + type);
    }

    Connections {
        target: connMan
        onContextAdded: {
            console.warn("Context added: " + path);
            updateContext(createApn(path));
        }
        onReportError: {
            console.warn("Connman error: " + errorMessage);
            // todo pop up an error dialog
        }
    }

    Connections {
        target: activeContexts
    }

    Component {
        id: apnContext
        OfonoContextConnection {
            onActiveChanged: {
                console.warn("Connection changed")
                console.warn("APN: " +apns.name)
            }
            onProvisioningFinished: {
                console.warn("Provisioned")
                console.warn("APN: " +apns.name)
            }
            onReportError: {
                console.warn("Context error: " + errorString)
                // todo pop up an error dialog
            }
        }
    }


    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: theContents.childrenRect.height + units.gu(5)
        //boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        //flickableDirection: Flickable.VerticalFlick

        Item {
            id: theContents
            anchors.fill: parent

            ListItem.Standard {
                id: sameSwitch
                anchors.top: parent.top
                text: i18n.tr("Same APN as for Internet")
                control: Switch {
                    id: doBoth
                    checked: false
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: {
                        // use the internet apn object
                        // we will not create a new one, just update the existing one
                        activeContext = activeContexts.internet;
                        initialize(activeContext);
                    }
                }
                visible: ismms
            }

            GridLayout {
                id: theGrid
                columns: 2
                columnSpacing: units.gu(1)
                rowSpacing: units.gu(1)
                anchors{
                    top: ismms ? sameSwitch.bottom : parent.top
                    right: parent.right
                    left:parent.left
                    margins: units.gu(0.5)
                }

                Label {
                    text: typeText + i18n.tr(" APN")
                }
                TextField {
                    id: apnName
                }

                Label {
                    text: i18n.tr("MMSC")
                    visible: ismms
                }
                TextField {
                    id: mmsc
                    visible: ismms
                }
                Label {
                    text: i18n.tr("Proxy")
                    visible: ismms
                }
                TextField {
                    id: proxy
                    visible: ismms
                }
                Label {
                    text: "Port"
                    visible: ismms
                }
                TextField {
                    id: port
                    visible: ismms
                    maximumLength: 4
                }

                Label {
                    text: i18n.tr("Username")
                }
                TextField {
                    id: userName
                }

                Label {
                    text: i18n.tr("Password")
                }
                TextField {
                    id: pword
                    echoMode: TextInput.PasswordEchoOnEdit
                }

                Label {
                    text: i18n.tr("Protocol")
                }
                OptionSelector {
                    id: protocol
                    //width: auth.width
                    model: [i18n.tr("IPv4"),
                        i18n.tr("IPv6"),
                        i18n.tr("IPv4IPv6")]
                }
            }
            Component.onCompleted: {
                var theContext;
                // this mess is to use either an existing custom context or
                // just copy values from the active context to make a new custom context
                if (ismms)
                    if (activeContexts.custommms) {
                        theContext = activeContexts.custommms
                        activeContext = theContext;
                    } else
                        theContext = activeContexts.mms;
                else
                   if (activeContexts.custominternet) {
                       theContext = activeContexts.custominternet
                       activeContext = theContext;
                   } else
                       theContext = activeContexts.internet;

                if (theContext) initialize(theContext);
            }

        } // the contents
    } // the flickable

    Item {
        id: buttonRectangle

        height: cancelButton.height + units.gu(2)

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Button {
            id: cancelButton

            text: i18n.tr("Cancel")

            anchors.left: parent.left
            anchors.right: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.topMargin: units.gu(1)
            anchors.leftMargin: units.gu(2)
            anchors.rightMargin: units.gu(1)
            anchors.bottomMargin: units.gu(1)

            onClicked: {
                pageStack.pop()
            }
        }

        Button {
            id: confirmButton

            text: i18n.tr("Set")
            anchors.left: parent.horizontalCenter
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.topMargin: units.gu(1)
            anchors.leftMargin: units.gu(1)
            anchors.rightMargin: units.gu(2)
            anchors.bottomMargin: units.gu(1)

            onClicked: {
                if (activeContext)
                    updateContext(activeContext);
                else
                    createContext();
                //pageStack.pop();
            }
        }
    } // item for buttons

}

