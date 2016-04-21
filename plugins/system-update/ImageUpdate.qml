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
    property var systemImageBackend
    formatter: Utilities.formatSize
    name: "Ubuntu"
    version: systemImageBackend.availableVersion
    size: systemImageBackend.updateSize
    iconUrl: "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"

    // onRetry: modelData.start();
    onDownload: systemImageBackend.downloadUpdate();
    onPause: systemImageBackend.pauseDownload();
    onInstall: systemImageBackend.applyUpdate();

    Connections {
        target: systemImageBackend
        onDownloadStarted: {
            update.progress = -1;
            update.status = UpdateManager.ManuallyDownloading;
            update.mode = UpdateManager.Pausable;
        }
        onUpdateProgress: {
            update.progress = percentage;
            update.status = UpdateManager.ManuallyDownloading;
            update.mode = UpdateManager.Pausable;
        }
        onUpdatePaused: {
            update.progress = percentage;
            update.status = UpdateManager.DownloadPaused;
            update.mode = UpdateManager.Resumable;
        }
        onUpdateDownloaded: {
            update.status = UpdateManager.NotStarted;
            update.mode = UpdateManager.InstallableWithRestart;
        }
        onUpdateFailed: {
            update.status = UpdateManager.Failed;
            update.mode = UpdateManager.Retriable;
            update.setError(i18n.tr("Download failed"), last_reason);
        }
    }
}
