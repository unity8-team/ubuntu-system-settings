/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Didier Roche <didier.roches@canonical.com>
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

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Update 1.0


ItemPage {
    id: root

    title: i18n.tr("Updates")
    flickable: scrollWidget // TODO: maybe remove

    // TODO: surely needs its own QML as the whole logic is here
    UbuntuUpdatePanel {
        id: updateBackend

        property int downloadProgress: 0
        property string downloadRemainingTime
        property string updateVersion
        property string updateSize
        property variant updateDescriptions: [""]

        // initial state
        property int currentUpdateState: UbuntuUpdatePanel.Checking
        property string checkinfoMessage: i18n.tr("Checking for updates")
        infoMessage: checkinfoMessage
        property string infoSecondaryMessage


        /***************************
         * DIRECT CALLS TO BACKEND *
         ***************************/
        function recheckForUpdate() {
            infoMessage = "";
            infoSecondaryMessage = "";
            var msg = CancelUpdate();
            if(msg) {
                infoMessage = TranslateFromBackend(msg);
                currentUpdateState = UbuntuUpdatePanel.CheckingError;
                return;
            }
            currentUpdateState = UbuntuUpdatePanel.Checking;
            infoMessage = checkinfoMessage;
            CheckForUpdate();
        }

        function downloadUpdate() {
            infoMessage = "";
            infoSecondaryMessage = "";
            DownloadUpdate();
        }

        function applyUpdate() {
            infoMessage = "";
            infoSecondaryMessage = "";
            var msg = ApplyUpdate();
            if (msg) {
                infoMessage = i18n.tr("Apply update failed:");
                infoSecondaryMessage = TranslateFromBackend(msg);
            }
        }

        function pauseDownload() {
            infoMessage = "";
            infoSecondaryMessage = "";
            var msg = PauseDownload();
            if (msg) {
                infoMessage = i18n.tr("Pause failed:");
                infoSecondaryMessage = TranslateFromBackend(msg);
            }
        }

        /************************
         * SIGNALS FROM BACKEND *
         ************************/
        onUpdateAvailableStatus: {

            infoMessage = "";
            updateVersion = availableVersion;
            var sizeInMB = updateSize/(1024*1024);
            if (sizeInMB > 1024)
                updateBackend.updateSize = i18n.tr("%1 GB").arg(sizeInMB/1024);
            else
                updateBackend.updateSize = i18n.tr("%1 MB").arg(sizeInMB);
            updateDescriptions = descriptions;

            if(isAvailable) {
                currentUpdateState = UbuntuUpdatePanel.UpdateAvailable;
            }
            else {
                currentUpdateState = UbuntuUpdatePanel.NoUpdate;
                infoMessage = i18n.tr("No software update available") + "<br/>" + i18n.tr("Last updated %1").arg(lastUpdateDate);
            }

            if(errorReason) {
                infoMessage = errorReason;
                currentUpdateState = UbuntuUpdatePanel.CheckingError;
            }
        }

        onUpdateProgress: {
            downloadProgress = percentage;
            if (eta > 0)
                downloadRemainingTime = i18n.tr("About %1 second remaining", "About %1 seconds remaining", eta).arg(eta);
            else
                downloadRemainingTime = i18n.tr("No estimate for the download");
            currentUpdateState = UbuntuUpdatePanel.Downloading;
        }

        onUpdatePaused: {
            downloadProgress = percentage;
            downloadRemainingTime = i18n.tr("Paused");
            currentUpdateState = UbuntuUpdatePanel.Paused;
        }

        onUpdateDownloaded: {
            currentUpdateState = UbuntuUpdatePanel.ReadyToInstall;
        }

        onUpdateFailed: {
            infoMessage = i18n.tr("Download failed:");
            infoSecondaryMessage = TranslateFromBackend(lastReason);
            currentUpdateState = UbuntuUpdatePanel.DownloadFailed;
        }

        function isUpdateContentToDisplay() {
            return currentUpdateState !== UbuntuUpdatePanel.Checking && currentUpdateState !== UbuntuUpdatePanel.NoUpdate && currentUpdateState !== UbuntuUpdatePanel.CheckingError;
        }
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            width: parent.width
            Item {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: units.gu(2)
                }
                height: {
                    if (updateBackend.isUpdateContentToDisplay())
                        return updateContentDisplay.height+ units.gu(4);
                    else {
                        var updateStatusSize = updateStatusbar.height + checkUpdateIndicator.height + units.gu(6);
                        if (retryCheckUpdateButton.visible)
                            return updateStatusSize + retryCheckUpdateButton.height
                        return updateStatusSize;
                    }
                }

                Column {
                    anchors.centerIn: parent
                    width: parent.width
                    spacing: units.gu(2)
                    visible: !updateBackend.isUpdateContentToDisplay()

                    ActivityIndicator {
                        id: checkUpdateIndicator
                        anchors.horizontalCenter: parent.horizontalCenter
                        running: updateBackend.currentUpdateState === UbuntuUpdatePanel.Checking
                        visible: running
                    }
                    Label {
                        id: updateStatusbar
                        anchors.horizontalCenter: parent.horizontalCenter
                        horizontalAlignment: Text.AlignHCenter
                        width: parent.width

                        text: updateBackend.infoMessage
                        visible: updateBackend.infoMessage
                        wrapMode: Text.WordWrap
                        textFormat: Text.RichText
                    }
                    Button {
                        id: retryCheckUpdateButton
                        text: i18n.tr("Retry")
                        width: parent.width
                        onClicked: updateBackend.recheckForUpdate()
                        visible: updateBackend.currentUpdateState === UbuntuUpdatePanel.NoUpdate || updateBackend.currentUpdateState === UbuntuUpdatePanel.CheckingError
                    }
                }

                Column {
                    id: updateContentDisplay
                    visible: updateBackend.isUpdateContentToDisplay()
                    width: parent.width
                    spacing: units.gu(2)

                    Icon {
                        id: distribLogo
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: units.gu(6)
                        height: width
                        name: "distributor-logo"
                    }

                    Row {
                        width: parent.width

                        Label {
                            width: parent.width/2
                            text: i18n.tr("Ubuntu Phone")
                        }

                        Label {
                            horizontalAlignment: Text.AlignRight
                            width: parent.width/2
                            text: updateBackend.updateSize;
                        }
                    }

                    // FIXME: use the right widget then
                    ListItem.ValueSelector {
                        text: i18n.tr("Version %1").arg(updateBackend.updateVersion)
                        values: updateBackend.updateDescriptions
                        selectedIndex: -1
                    }

                    ListItem.Subtitled {
                        text: updateBackend.infoMessage
                        subText: updateBackend.infoSecondaryMessage
                        visible: updateBackend.infoMessage !== ""
                    }

                    Column {
                        id: updateDownloading
                        spacing: units.gu(1)
                        visible: updateBackend.currentUpdateState === UbuntuUpdatePanel.Downloading || updateBackend.currentUpdateState === UbuntuUpdatePanel.Paused
                        width: parent.width

                        ProgressBar {
                            id: updateProgress
                            maximumValue : 100
                            minimumValue : 0
                            value : updateBackend.downloadProgress
                            width: parent.width
                        }

                        Label {
                            text: updateBackend.downloadRemainingTime
                            visible: updateBackend.currentUpdateState === UbuntuUpdatePanel.Downloading || updateBackend.currentUpdateState === UbuntuUpdatePanel.Paused
                        }

                        Button {
                            id: pauseDownloadButton
                            text: i18n.tr("Pause downloading")
                            width: parent.width
                            onClicked: updateBackend.pauseDownload()
                            visible: updateBackend.currentUpdateState === UbuntuUpdatePanel.Downloading
                        }

                        Button {
                            text: i18n.tr("Resume downloading")
                            width: parent.width
                            onClicked: updateBackend.downloadUpdate()
                            visible: updateBackend.currentUpdateState === UbuntuUpdatePanel.Paused
                        }

                    }

                    Column {
                        id: updateStopped
                        spacing: units.gu(1)
                        width: parent.width

                        Button {
                            text: i18n.tr("Download")
                            width: parent.width
                            onClicked: updateBackend.downloadUpdate()
                            visible: updateBackend.currentUpdateState === UbuntuUpdatePanel.UpdateAvailable
                        }

                        Button {
                            text: i18n.tr("Retry")
                            width: parent.width
                            onClicked: updateBackend.recheckForUpdate()
                            visible: updateBackend.currentUpdateState === UbuntuUpdatePanel.DownloadFailed
                        }

                        Button {
                            text: i18n.tr("Install & Restart")
                            width: parent.width
                            onClicked: updateBackend.applyUpdate()
                            visible: updateBackend.currentUpdateState === UbuntuUpdatePanel.ReadyToInstall
                        }
                    }
                }
            }

            ListItem.Standard {
                text: i18n.tr ("Download future updates automatically:")
            }
            // TODO: this widget needs to be replace by the real one
            ListItem.ValueSelector {
                id: upgradePolicySelector
                expanded: true
                onExpandedChanged: expanded = true
                values: [i18n.tr("Never"),
                    i18n.tr("When on wi-fi"),
                    // TODO: Data charges may apply needs to be smaller
                    i18n.tr("On any data connection\nData charges may apply.")]
                selectedIndex: updateBackend.downloadMode
                onSelectedIndexChanged: updateBackend.downloadMode = selectedIndex
            }
        }
    }
}
