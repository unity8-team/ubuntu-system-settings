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
    property string packageName

    signal requestedRetry(string packageName)

    Component.onCompleted: {
        console.warn("ClickUpdate", modelData, modelData.metadata, modelData.metadata.title, modelData.metadata.custom.iconUrl);
        packageName = modelData.metadata.custom["package-name"];
        console.warn("have package name", packageName);
    }
    // Initial status, mode
    status: UpdateManager.NotStarted
    mode: UpdateManager.Installable

    name: modelData.metadata.title
    version: modelData.metadata.custom.remoteVersion
    size: modelData.metadata.custom.binaryFilesize
    iconUrl: modelData.metadata.custom.iconUrl
    changelog: modelData.metadata.custom.changelog
    progress: modelData.progress

    // If this failed, we tell our parents
    onRetry: requestedRetry(update.packageName)

    onDownload: modelData.start();
    onPause: modelData.pause();
    onInstall: modelData.start();

    Connections {
        target: modelData
        onErrorMessageChanged: {
            update.setError(
                i18n.tr("Download failed"), modelData.errorMessage
            )
            update.mode = UpdateManager.Retriable;
            update.status = UpdateManager.Failed;
        }
        onFinished: {
            console.warn('ClickUpdate Connections: Download finished')
            update.mode = UpdateManager.NonPausable;
            update.status = UpdateManager.Installed;
        }
        onProgressChanged: {
            console.warn('ClickUpdate Connections: Progress changed', progress)
            update.mode = UpdateManager.Pausable;
            update.status = UpdateManager.ManuallyDownloading;
        }
    }
}
