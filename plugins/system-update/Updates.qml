/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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
 * This file represents the UI of the System Updates panel.
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.Update 1.0

Column {
    id: updates
    property bool havePower: false
    property bool haveSystemUpdate: false
    property int managerStatus
    property int updatesCount
    property bool authenticated: false
    property bool online: false
    property alias systemImageBackend: imageUpdate.systemImageBackend
    property alias clickUpdatesModel: clickUpdates.model

    signal requestAuthentication()
    signal stop()
    signal udmDownloadCreated(string packageName, int revision, int udmId)

    Global {
        id: glob
        anchors {
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }

        height: hidden ? 0 : units.gu(6)
        managerStatus: updates.managerStatus
        requireRestart: updates.haveSystemUpdate
        updatesCount: updates.updatesCount
        online: updates.online

        onStop: updates.stop()
    }

    SystemUpdates {
        id: systemUpdate
        anchors { left: parent.left; right: parent.right }
        visible: (updates.haveSystemUpdate
                  && updates.managerStatus === UpdateManager.Idle)
    }

    ClickUpdates {
        id: clickUpdates
        anchors { left: parent.left; right: parent.right }
        visible: (authenticated
                  && updates.managerStatus === UpdateManager.Idle)
        onRequestedRetry: clickPackageRequestedRetry(packageName)
    }

    NotAuthenticatedNotification {
        visible: !authenticated
        onRequestAuthentication: updates.requestAuthentication()
    }


    Rectangle {
        color: "white"
        anchors.fill: parent
        visible: placeholder.text

        Label {
            id: placeholder
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            text: {
                var s = UpdateManager.managerStatus;
                if (!UpdateManager.online) {
                    return i18n.tr("Connect to the Internet to check for updates.");
                } else if (s === UpdateManager.Idle && UpdateManager.updatesCount === 0) {
                    return i18n.tr("Software is up to date");
                } else if (s === UpdateManager.ServerError || s === UpdateManager.NetworkError) {
                    return i18n.tr("The update server is not responding. Try again later.");
                }
                return "";
            }
        }
    }
}
