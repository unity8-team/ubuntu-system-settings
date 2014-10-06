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
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem

ItemPage {
    objectName: "customapnPage"
    id: root

    // "internet" or "mms"
    property var type

    // dict of "type" : ctx
    property var contexts;

    /// work around LP(#1361919)
    property var activateCb;

    QtObject {
        id: d
        property var typeText : type === "internet" ? i18n.tr("Internet") : i18n.tr("MMS")
        property bool isMms : type === "mms"

        property bool isValid : false

        function validateFields() {
            if (apnName.text === "") {
                isValid = false;
                return
            }
            if (isMms) {
                if (mmsc.text === "") {
                    isValid = false;
                    return;
                }
                /// @todo validate proxy
                /// @todo force port to be integer and validate it's value
            }

            // @todo the rest
            isValid = true;
        }
    }

    //TRANSLATORS: %1 is either i18n.tr("Internet") or i18n.tr("MMS")
    title: i18n.tr("Custom %1 APN").arg(d.typeText)

    // workaround of getting the following error on startup:
    // WARNING - ... : QML Page: Binding loop detected for property "flickable"
    flickable: null
    Component.onCompleted: {
        flickable: scrollWidget

        var ctx;
        if (d.isMms) {
            ctx = contexts["mms"];
            if (ctx === undefined) {
                // @bug LP(:#1362795)
                return;
            }
        } else {
            ctx = contexts["internet"]
        }

        apnName.text = ctx.accessPointName;
        userName.text = ctx.username;
        pword.text = ctx.password;
        mmsc.text = ctx.messageCenter;
        var proxyText = ctx.messageProxy.split(":");
        proxy.text = proxyText[0] !== undefined ? proxyText[0] : "";
        port.text = proxyText[1] !== undefined ? proxyText[1] : "";
        /// @todo protocol values

        if (d.isMms) {
            /// @todo disabled for now
            doBoth.checked = false;
            return;
            var internetApn = contexts["internet"]
            if (ctx.accessPointName === internetApn.accessPointName &&
                ctx.username == internetApn.username &&
                ctx.password == internetApn.password
                /* auth + procol */) {
                doBoth.checked = true;
            }
            else
                doBoth.checked = false;
        }
    }

    Flickable {
        id: scrollWidget
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        contentWidth: parent.width
        clip: true
        contentHeight: theContents.height
        boundsBehavior: (contentHeight > height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick

        Item {
            id: theContents
            height: sameSwitch.height + theGrid.height
            width: parent.width

            ListItem.Standard {
                id: sameSwitch
                anchors.top: parent.top
                /// @todo disable for now
                //visible: d.isMms
                visible: false
                text: i18n.tr("Same APN as for Internet")
                control: Switch {
                    id: doBoth
                    checked: false
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: {
                        if (checked) {
                            var internetApn = contexts["internet"]
                            apnName.text = internetApn.accessPointName;
                            userName.text = internetApn.username;
                            pword.text = internetApn.password;
                        }
                    }
                }
            }

            GridLayout {
                id: theGrid
                columns: 2
                columnSpacing: units.gu(1)
                rowSpacing: units.gu(1)
                anchors{
                    top: d.isMms ? sameSwitch.bottom : parent.top
                    right: parent.right
                    left:parent.left
                    margins: units.gu(0.5)
                }

                Label {
                    //TRANSLATORS: %1 is either i18n.tr("Internet") or i18n.tr("MMS")
                    text: i18n.tr("%1 APN").arg(d.typeText)
                }
                TextField {
                    id: apnName
                    enabled: !doBoth.checked
                    onTextChanged: d.validateFields()
                    inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                }

                Label {
                    text: i18n.tr("MMSC")
                    visible: d.isMms
                }
                TextField {
                    id: mmsc
                    visible: d.isMms
                    onTextChanged: d.validateFields()
                    inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                }
                Label {
                    text: i18n.tr("Proxy")
                    visible: d.isMms
                }
                TextField {
                    id: proxy
                    visible: d.isMms
                    onTextChanged: d.validateFields()
                    inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                }
                Label {
                    text: "Port"
                    visible: d.isMms
                }
                TextField {
                    id: port
                    visible: d.isMms
                    maximumLength: 4
                    onTextChanged: d.validateFields()
                    inputMethodHints: Qt.ImhDigitsOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                }

                Label {
                    text: i18n.tr("Username")
                }
                TextField {
                    id: userName
                    enabled: !doBoth.checked
                    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                }

                Label {
                    text: i18n.tr("Password")
                }
                TextField {
                    id: pword
                    enabled: !doBoth.checked
                    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                }
                /// @todo support for ipv6 will be added after RTM
            }
            Item {
                id: buttonRectangle

                height: cancelButton.height + units.gu(2)

                anchors {
                    left: parent.left
                    right: parent.right
                    top: theGrid.bottom
                }

                Button {
                    id: cancelButton

                    text: i18n.tr("Cancel")

                    anchors {
                        left: parent.left
                        right: parent.horizontalCenter
                        bottom: parent.bottom
                        topMargin: units.gu(1)
                        leftMargin: units.gu(2)
                        rightMargin: units.gu(1)
                        bottomMargin: units.gu(1)
                    }

                    onClicked: {
                        pageStack.pop()
                    }
                }

                Button {
                    id: confirmButton

                    text: d.isMms ? i18n.tr("Save") : i18n.tr("Activate")

                    anchors {
                        left: parent.horizontalCenter
                        right: parent.right
                        bottom: parent.bottom
                        topMargin: units.gu(1)
                        leftMargin: units.gu(1)
                        rightMargin: units.gu(2)
                        bottomMargin: units.gu(1)
                    }

                    enabled: d.isValid;

                    onClicked: {
                        var ctx;
                        if (d.isMms)
                            ctx = contexts["mms"];
                        else
                            ctx = contexts["internet"];

                        /// @bug LP(:#1362795)
                        if (d.isMms && ctx === undefined) {
                            var mmsData = ({})
                            mmsData["accessPointName"] = apnName.text;
                            mmsData["username"] = userName.text;
                            mmsData["password"] = pword.text;
                            mmsData["messageCenter"] = mmsc.text
                            var proxyValue = "";
                            if (proxy.text !== "") {
                                proxyValue = proxy.text;
                                if (port.text !== "")
                                    proxyValue = proxyValue + ":" + port.text;
                            }
                            mmsData["messageProxy"] = proxyValue;
                            activateCb("mms", undefined, mmsData);
                            pageStack.pop();
                            return;
                        }

                        ctx.accessPointName = apnName.text;
                        ctx.username = userName.text;
                        ctx.password = pword.text;
                        if (d.isMms) {
                            ctx.messageCenter = mmsc.text;
                            var proxyValue = "";
                            if (proxy.text !== "") {
                                proxyValue = proxy.text;
                                if (port.text !== "")
                                    proxyValue = proxyValue + ":" + port.text;
                            }
                            ctx.messageProxy = proxyValue
                        }
                        /// @todo map protocol values

                        activateCb(ctx.type, ctx.contextPath);
                        pageStack.pop();
                    }
                }
            } // item for buttons
        } // the contents
    } // the flickable
}
