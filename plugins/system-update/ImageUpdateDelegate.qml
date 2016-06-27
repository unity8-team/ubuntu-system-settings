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
    name: "Ubuntu Touch"
    version: SystemImage.availableVersion
    size: SystemImage.updateSize
    iconUrl: "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"
    updateState: SystemImage.checkTarget() ? Update.StateAvailable :
                                              Update.StateUnavailable
    // onRetry: modelData.downloadUpdate();
    onDownload: SystemImage.downloadUpdate();
    onPause: SystemImage.pauseDownload();
    // onInstall: SystemImage.applyUpdate();

    Connections {
        target: SystemImage
        onDownloadStarted: {
            update.progress = -1;
            switch (SystemImage.downloadMode) {
                case 0: // Manual
                    update.updateState = Update.StateQueuedForDownload;
                    break;
                case 1: // Auto on Wi-Fi
                case 2: // Always
                    update.updateState = Update.StateDownloadingAutomatically;
                    break;
            }
        }
        onUpdateProgress: {
            update.progress = percentage;
            switch (SystemImage.downloadMode) {
                case 0: // Manual
                    update.updateState = Update.StateDownloading;
                    break;
                case 1: // Auto on Wi-Fi
                case 2: // Always
                    update.updateState = Update.StateDownloadingAutomatically;
                    break;
            }
        }
        onUpdatePaused: {
            update.progress = percentage;
            switch (SystemImage.downloadMode) {
                case 0: // Manual
                    update.updateState = Update.StateDownloadPaused;
                    break;
                case 1: // Auto on Wi-Fi
                case 2: // Always
                    update.updateState = Update.StateAutomaticDownloadPaused;
                    break;
            }
        }
        onUpdateDownloaded: {
            update.updateState = Update.StateDownloaded;
        }
        onUpdateFailed: {
            update.updateState = Update.StateFailed;
            update.setError(i18n.tr("Update failed"), lastReason);
        }
        onDownloadModeChanged: {
            // Pause an automatic download if the downloadMode changes to Manual.
            if (SystemImage.downloadMode === 0 &&
                update.updateState === Update.StateDownloadingAutomatically) {
                update.pause();
            }
        }
    }
}
