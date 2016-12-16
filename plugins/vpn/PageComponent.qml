/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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

import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.Components.Popups 1.3
import Ubuntu.Connectivity 1.0
import Ubuntu.Settings.Vpn 0.1

ItemPage {
    id: root
    title: i18n.tr("VPN")
    objectName: "vpnPage"
    flickable: scrollWidget

    property var diag

    function openConnection(connection, isNew) {
        pageStack.addPageToNextColumn(root, vpnEditorDialog, {
            "connection": connection,
            "isNew": isNew
        });
    }

    function previewConnection(connection) {
        diag = PopupUtils.open(vpnPreviewDialog, root, {"connection": connection});
    }

    Flickable {
        id: scrollWidget
        anchors {
            fill: parent
            topMargin: units.gu(1)
            bottomMargin: units.gu(1)
        }
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors { left: parent.left; right: parent.right }

            VpnList {
                id: list
                anchors { left: parent.left; right: parent.right }
                model: Connectivity.vpnConnections

                onClickedConnection: previewConnection(connection)
            }

            ListItem.Caption {
                // We do not yet support configuration files.
                visible: false
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text : i18n.tr("To add a VPN configuration, download its config file or configure it manually.")
            }

            ListItem.SingleControl {
                control: Button {
                    objectName: "addVpnButton"
                    text : i18n.tr("Add Manual Configuration…")
                    onClicked: Connectivity.vpnConnections.add(VpnConnection.OPENVPN)
                }
            }
        }
    }

    Component {
        id: vpnEditorDialog
        VpnEditor {
            id: vpnEditorPage
            onTypeChanged: {
                connection.remove();
                pageStack.removePages(vpnEditorPage);
                Connectivity.vpnConnections.add(type);
            }
            onReconnectionPrompt: PopupUtils.open(reconnPrompt)
            onDone: pageStack.removePages(vpnEditorPage)
        }
    }

    Component {
        id: vpnPreviewDialog
        VpnPreviewDialog {
            onChangeClicked: {
                PopupUtils.close(diag);
                openConnection(connection);
            }
        }
    }

    Component {
        id: reconnPrompt
        Dialog {
            id: reconnPromptDiag
            title: i18n.tr("VPN reconnection required.")
            text: i18n.tr("You need to reconnect for changes to have an effect.")

            ListItem.SingleControl {
                control: Button {
                    width: parent.width
                    text : i18n.tr("OK")
                    onClicked: PopupUtils.close(reconnPromptDiag);
                }
            }
        }
    }

    Connections {
        target: Connectivity.vpnConnections
        onAddFinished: openConnection(connection, true)
    }
}
