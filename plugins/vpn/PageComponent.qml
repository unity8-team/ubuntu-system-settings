/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * Contact: Ken VanDine <jonas.drange@canonical.com>
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

    function openConnection(connection) {
        PopupUtils.open(vpnEditorDialog, root, {"connection": connection});
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
            }

            ListItem.Caption {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text : i18n.tr("To add a VPN configuration, download its config file or configure it manually.")
            }

            ListItem.SingleControl {
                control: Button {
                    text : i18n.tr("Add Manual Configuration…")
                    onClicked: Connectivity.vpnConnections.add(VpnConnection.OPENVPN)
                }
            }
        }
    }

    // FIXME: Load this async
    Component {
        id: vpnEditorDialog
        VpnEditorDialog {}
    }

    Connections {
        target: Connectivity.vpnConnections
        onAddFinished: openConnection(connection)
    }
}
