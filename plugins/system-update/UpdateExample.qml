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
    width: units.gu(35)
    height: units.gu(90)

    Update {
        anchors { left: parent.left; right: parent.right }
        name: "Ubuntu"
        iconUrl: "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"
        changelog: "* Add audio & video policy groups by default.\n* Enable back & forward buttons.\n* Switch to wikidata"
        size: 3303
        version: "0.5"
        progress: 50
        mode: UpdateManager.Downloadable
        status: UpdateManager.NotStarted
    }
    Update {
        anchors { left: parent.left; right: parent.right }
        name: "Spotify"
        iconUrl: "https://lh3.googleusercontent.com/UrY7BAZ-XfXGpfkeWg0zCCeo-7ras4DCoRalC_WXXWTK9q5b0Iw7B0YQMsVxZaNB7DM=w300"
        changelog: ""
        size: 875421
        version: "0.5"
        progress: 50
        mode: UpdateManager.Installable
        status: UpdateManager.NotStarted
        onInstall: {
            if (status == UpdateManager.NotStarted) {
                status = UpdateManager.Installing
            } else if (status == UpdateManager.Installing) {
                status = UpdateManager.Installed
                mode = UpdateManager.NonPausable
           }
       }
    }
    Update {
        anchors { left: parent.left; right: parent.right }
        name: "Castle Defence System & Dragons & Dungeons"
        iconUrl: "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"
        changelog: "* Add audio & video policy groups by default.\n* Enable back & forward buttons.\n* Switch to wikidata * Add audio & video policy groups by default.\n* Enable back & forward buttons.\n* Switch to wikidata"
        size: 3758
        version: "0.5"
        progress: 50
        mode: UpdateManager.NonPausable
        status: UpdateManager.Installing
    }
    Update {
        anchors { left: parent.left; right: parent.right }
        name: "FooDab"
        iconUrl: "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"
        changelog: "* Add audio & video policy groups by default.\n* Enable back & forward buttons.\n* Switch to wikidata"
        size: 98987
        version: "0.5"
        progress: 37
        mode: UpdateManager.Pausable
        status: UpdateManager.AutomaticallyDownloading
    }
    Update {
        anchors { left: parent.left; right: parent.right }
        name: "Bad Seed"
        iconUrl: "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"
        changelog: "* Add audio & video policy groups by default.\n* Enable back & forward buttons.\n* Switch to wikidata"
        size: 50648
        version: "0.5"
        progress: 80
        mode: UpdateManager.Retriable
        status: UpdateManager.Failed
        Component.onCompleted: setError("Download failed", "There was a damned issue. The update server is not responding. Please try again later.")
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
