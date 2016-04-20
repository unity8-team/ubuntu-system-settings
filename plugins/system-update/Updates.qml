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

Column {
    id: updates

    /*!
        \qmlproperty bool havePower

        Indicates whether or not there is sufficient power to perform
        updates. If disabled, it causes the UI to prompt for power to
        be provided if user initiates an update.
    */
    property bool havePower: false

    /*!
        \qmlproperty bool haveSystemUpdate

        Indicates whether or not there exist a system update.
    */
    property bool haveSystemUpdate: false

    /*!
        \qmlproperty UpdateManager::ManagerStatus managerStatus

        Reflects state of manager.
    */
    property int managerStatus

    /*!
        \qmlproperty int updatesCount

        Amount of updates.
    */
    property int updatesCount

    /*!
        \qmlproperty bool authenticated

        Indicates whether or not the user is authenticated.
    */
    property bool authenticated: false

    /*!
        \qmlproperty bool online

        Indicates whether or not we're connected to the internet.
    */
    property bool online: false

    /*!
        \qmlproperty alias systemImageBackend

        Holds the system image backend.
    */
    property alias systemImageBackend: imageUpdate.systemImageBackend

    /*!
        \qmlproperty alias clickUpdatesModel

        Holds the click updates model. Normally this is UDM.downloads.
    */
    property alias clickUpdatesModel: clickUpdates.model

    /*!
        \qmlsignal requestAuthentication

        Indicates that the user wanted to authenticate.

        \sa authenticated
    */
    signal requestAuthentication()

    /*!
        \qmlsignal stop

        Indicates that the user wants some action to stop.
        Note: Currently this is only implemented for checks.
    */
    signal stop()

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

    ImageUpdate {
        id: imageUpdate
        anchors { left: parent.left; right: parent.right }
        visible: updates.haveSystemUpdate
    }

    ClickUpdates {
        id: clickUpdates
        anchors { left: parent.left; right: parent.right }
        visible: authenticated
    }

    NotAuthenticatedNotification {
        visible: !authenticated
        onRequestAuthentication: updates.requestAuthentication()
    }
}
