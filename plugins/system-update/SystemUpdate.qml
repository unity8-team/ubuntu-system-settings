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

Update {
    id: update
    property var backend
    name: "Ubuntu Touch"
    version: backend.availableVersion
    size: backend.updateSize
    iconUrl: "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"

    // onRetry: modelData.start();
    onDownload: backend.downloadUpdate();
    onPause: backend.pauseDownload();
    onInstall: backend.applyUpdate();

    Connections {
        target: backend
        onDownloadStarted: {
            update.progress = -1;
            switch (backend.downloadMode) {
                case 0: // Manual
                    update.updateState = UpdateManager.StateQueuedForDownload;
                    break;
                case 1: // Auto on Wi-Fi
                case 2: // Always
                    update.updateState = UpdateManager.StateDownloadingAutomatically;
                    break;
            }
        }
        onUpdateProgress: {
            update.progress = percentage;
            switch (backend.downloadMode) {
                case 0: // Manual
                    update.updateState = UpdateManager.StateDownloading;
                    break;
                case 1: // Auto on Wi-Fi
                case 2: // Always
                    update.updateState = UpdateManager.StateDownloadingAutomatically;
                    break;
            }
        }
        onUpdatePaused: {
            update.progress = percentage;
            switch (backend.downloadMode) {
                case 0: // Manual
                    update.updateState = UpdateManager.StateDownloadPaused;
                    break;
                case 1: // Auto on Wi-Fi
                case 2: // Always
                    update.updateState = UpdateManager.StateAutomaticDownloadPaused;
                    break;
            }
        }
        onUpdateDownloaded: {
            update.updateState = UpdateManager.StateDownloaded;
        }
        onUpdateFailed: {
            update.updateState = UpdateManager.StateFailed;
            update.setError(i18n.tr("Update failed"), lastReason);
        }
        onDownloadModeChanged: {
            console.log("onDownloadModeChanged", backend.downloadMode);
            // Pause an automatic download if the downloadMode changes to Manual.
            if (backend.downloadMode === 0 &&
                update.updateState === UpdateManager.StateDownloadingAutomatically) {
                update.pause();
            }
        }
    }
}
