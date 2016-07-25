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
 */

import QtQuick 2.4
import Ubuntu.SystemSettings.Update 1.0

UpdateDelegate {
    id: update

    /* Convert paused download to a paused automatic download if this
    download (most likely) was started automatically. */
    Binding {
        target: update
        property: "updateState"
        value: Update.StateAutomaticDownloadPaused
        when: {
            var autoMode = SystemImage.downloadMode > 0;
            var paused = false;
            switch (model.updateState) {
            case Update.StateDownloadPaused:
                paused = true;
            }
            return autoMode && paused;
        }
    }

    /* Convert download to an automatic download if this
    download (most likely) was started automatically. */
    Binding {
        target: update
        property: "updateState"
        value: Update.StateDownloadingAutomatically
        when: {
            var autoMode = SystemImage.downloadMode > 0;
            var downloading = false;
            switch (model.updateState) {
            case Update.StateQueuedForDownload:
            case Update.StateDownloading:
                downloading = true;
            }
            return autoMode && downloading;
        }
    }
}
