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

    width: units.gu(50)
    height: units.gu(95)

    /*!
        \qmlproperty bool havePower

        Indicates whether or not there is sufficient power to perform
        updates. If disabled, it causes the UI to prompt for power to
        be provided if user initiates an update.
    */
    property bool havePower: false

    /*!
        \qmlproperty UpdateManager::ManagerStatus managerStatus

        Reflects state of manager.
    */
    property int managerStatus

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

    Global {
        id: globalArea
    }

    ImageUpdate {
        id: imageUpdate
        // FIXME: remove
        visible: false
    }

    ClickUpdates {
        id: clickUpdates
        visible: authenticated
    }

    NotAuthenticatedNotification {
        visible: !authenticated
        onRequestAuthentication: updates.requestAuthentication()
    }
}
