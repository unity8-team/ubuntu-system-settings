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
 * Visually represents an update, and accepts user input. It does NOT
 * know how to perform downloads and updates:
 * This component is meant to be software agnostic, meaning it doesn't
 * care whether or not it is a Click/Snappy/System update.
 */
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.Update 1.0

import "."

Column {
    width: units.gu(50)
    height: units.gu(90)

    Update {
        anchors { left: parent.left; right: parent.right }
        name: "Ubuntu"
        iconUrl: "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"
        changelog: "* Add audio & video policy groups by default.\n* Enable back & forward buttons.\n* Switch to wikidata"
        size: "300.3 MB"
        version: "0.5"
        progress: 50
        mode: UpdateManager.Pausable
        status: UpdateManager.AutomaticallyDownloading
    }
    Update {
        anchors { left: parent.left; right: parent.right }
        name: "Ubuntu"
        iconUrl: "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"
        changelog: "* Add audio & video policy groups by default.\n* Enable back & forward buttons.\n* Switch to wikidata"
        size: "300.3 MB"
        version: "0.5"
        progress: 50
        mode: UpdateManager.NonPausable
        status: UpdateManager.Installing
    }
}

        // UpdateDownloadable,
        // UpdateInstallable,
        // UpdateInstallableWithRestart,
        // UpdatePausable,
        // UpdateNonPausable,
        // UpdateRetriable

        // UpdateNotAvailable,
        // UpdateNotStarted,
        // UpdateAutomaticallyDownloading,
        // UpdateManuallyDownloading,
        // UpdateDownloadPaused,
        // UpdateInstallationPaused,
        // UpdateInstalling,
        // UpdateInstalled,
        // UpdateFailed
