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
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import MeeGo.QOfono 0.2
import Ubuntu.SystemSettings.Cellular 1.0

ItemPage {
    objectName: "viewapnPage"
    id: root

    property var typeText
    property var type
    property var ismms
    property variant theContext
    property variant connMan

    type: theContext.type
    typeText: type === "internet" ? i18n.tr("Internet") : i18n.tr("MMS")
    ismms: type === "mms" || theContext.messageCenter !== ""
    title: typeText + i18n.tr(" APN")

    flickable: scrollWidget

    OfonoActivator {
        id:activator
    }

    function initialize() {

        console.warn("setting fields")
        // prepopulate the fields
        apnName.text = theContext.accessPointName;
        userName.text = theContext.username;
        pword.text = theContext.password;
        // todo map protocol values
        protocol.text = theContext.protocol;
        if (ismms) {
            mmsc.text = theContext.messageCenter;
            var proxyText = theContext.messageProxy.split(":")
            proxy.text = proxyText[0];
            if (proxyText[1]) port.text = proxyText[1];
        }

    }

    Connections {
        target: connMan
        onReportError: {
            console.warn("Connman error: " + errorMessage);
            // todo pop up an error dialog
        }
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: theContents.childrenRect.height + units.gu(5)

        Item {
            id: theContents
            anchors.fill: parent

            GridLayout {
                id: theGrid
                columns: 2
                columnSpacing: units.gu(1)
                rowSpacing: units.gu(1)
                anchors{
                    top: parent.top
                    right: parent.right
                    left:parent.left
                    margins: units.gu(0.5)
                }

                Label {
                    text: typeText + i18n.tr(" APN")
                }
                Label {
                    id: apnName
                }

                Label {
                    text: i18n.tr("MMSC")
                    visible: ismms
                }
                Label {
                    id: mmsc
                    visible: ismms
                }
                Label {
                    text: i18n.tr("Proxy")
                    visible: ismms
                }
                Label {
                    id: proxy
                    visible: ismms
                }
                Label {
                    text: "Port"
                    visible: ismms
                }
                Label {
                    id: port
                    visible: ismms
                }

                Label {
                    text: i18n.tr("Username")
                }
                Label {
                    id: userName
                }

                Label {
                    text: i18n.tr("Password")
                }
                Label {
                    id: pword
                }

                Label {
                    text: i18n.tr("Protocol")
                }
                Label {
                    id: protocol
                }
            }
            Component.onCompleted: {
                initialize();
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
                if (!theContext.active) {
                    activator.activate(theContext.contextPath);
                }
                pageStack.pop();
            }
        }
    } // item for buttons

}

