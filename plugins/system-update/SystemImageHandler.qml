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
 */
import QtQuick 2.4
import Ubuntu.SystemSettings.Update 1.0

Item {
    property var updateModel

    Connections {
        target: SystemImage

        function retry() {
            SystemImage.downloadUpdate();
        }
        function download() {
            SystemImage.downloadUpdate();
        }
        function pause() {
            var result = SystemImage.pauseDownload();
            if (result) {
                console.warn('Failed to pause image download', result);
            }
        }

        function install() {
            SystemImage.applyUpdate();
        }

        function markInstalled(buildNumber) {
            updateModel.setInstalled("ubuntu", buildNumber);
        }

        onUpdateAvailableStatus: {
            // console.warn("onUpdateAvailableStatus",
            //              isAvailable,
            //              downloading,
            //              availableVersion,
            //              updateSize,
            //              lastUpdateDate,
            //              errorReason);
            if (isAvailable)
                updateModel.setImageUpdate("ubuntu", availableVersion, updateSize);
                if (downloading)
                    updateModel.startUpdate("ubuntu", availableVersion);
                else if (errorReason === "paused")
                    updateModel.pauseUpdate("ubuntu", availableVersion);
        }
        onDownloadStarted: {
            updateModel.setProgress("ubuntu", SystemImage.targetBuildNumber, 0);
        }
        onUpdateProgress: {
            updateModel.setProgress("ubuntu", SystemImage.targetBuildNumber, percentage);
        }
        onUpdatePaused: {
            updateModel.setProgress("ubuntu", SystemImage.targetBuildNumber, percentage);
            updateModel.pauseUpdate("ubuntu", SystemImage.targetBuildNumber);
        }
        onUpdateDownloaded: {
            updateModel.setDownloaded("ubuntu", SystemImage.targetBuildNumber);
        }
        onUpdateFailed: {
            updateModel.setError("ubuntu", SystemImage.targetBuildNumber, lastReason);
        }

        /* This is currently the best we can do for marking image updates as
        installed. lp:1600449 */
        onCurrentBuildNumberChanged: {
            markInstalled(SystemImage.currentBuildNumber);
        }
        onRebooting: {
            if (status) {
                updateModel.setInstalling("ubuntu", SystemImage.targetBuildNumber, 5);
            } else {
                updateModel.setError("ubuntu", SystemImage.targetBuildNumber,
                                     i18n.tr("Failed to restart device."));
            }
        }
        onUpdateProcessing: {
            updateModel.setInstalling("ubuntu", SystemImage.targetBuildNumber);
        }
        onUpdateProcessFailed: {
                updateModel.setError("ubuntu", SystemImage.targetBuildNumber,
                                     i18n.tr("Failed to process update."));
        }
        Component.onCompleted: {
            markInstalled(SystemImage.currentBuildNumber);
        }
    }
}
