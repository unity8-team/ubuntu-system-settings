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
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Connectivity 1.0
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Components.Popups 1.3
import Ubuntu.DownloadManager 1.2
import Ubuntu.SystemSettings.Update 1.0

Item {
    id: updates
    property var clickManager
    property var clickModel
    property var imageModel
    property var installedModel
    property var downloadHandler

    property Flickable flickable: scrollWidget
    property bool havePower: false
    property bool online: NetworkingStatus.online
    property bool haveSystemUpdate: SystemImage.checkTarget()
    property bool authenticated: true
    property int status: SystemUpdate.StatusIdle
    property int updatesCount: {
        var count = 0;
        if (authenticated) {
            count += clickModel.count;
        }
        count += imageModel.count;
        return count;
    }

    function checkSystem() {
        SystemImage.checkForUpdate();

        switch (updates.status) {
        case SystemUpdate.StatusCheckingClickUpdates:
            updates.status = SystemUpdate.StatusCheckingAllUpdates; break;
        case SystemUpdate.StatusIdle:
            updates.status = SystemUpdate.StatusCheckingSystemUpdates; break;
        }
    }

    function checkClick() {
        clickManager.check();
    }

    function cancelChecks() {
        clickManager.cancel();
        updates.status = SystemUpdate.StatusIdle;
    }

    signal requestAuthentication()

    states: [
        State {
            name: "offline"
            PropertyChanges { target: overlay; visible: true }
            PropertyChanges { target: glob; hidden: true }
            PropertyChanges { target: imageUpdate; visible: false }
            PropertyChanges { target: clickUpdates; visible: false }
            PropertyChanges { target: updatesAvailableHeader; visible: false }
            when: !online
        },
        State {
            name: "error"
            PropertyChanges { target: overlay; visible: true }
            PropertyChanges { target: glob; hidden: true }
            PropertyChanges { target: imageUpdate; visible: false }
            PropertyChanges { target: clickUpdates; visible: false }
            PropertyChanges { target: updatesAvailableHeader; visible: false }
            when: status === SystemUpdate.StatusNetworkError ||
                  status === SystemUpdate.StatusServerError
        },
        State {
            name: "noAuth"
            when: !updates.authenticated
            PropertyChanges { target: notauthNotification; visible: true }
            PropertyChanges { target: clickUpdates; visible: false }
        },
        State {
            name: "noUpdates"
            PropertyChanges { target: overlay; visible: true }
            when: {
                var idle = status === SystemUpdate.StatusIdle;
                var noUpdates = (updatesCount === 0) && !updates.haveSystemUpdate;
                return idle && noUpdates;
            }
        }
    ]

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: content.height
        boundsBehavior: (contentHeight > parent.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick
        clip: true

        Column {
            id: content
            anchors { left: parent.left; right: parent.right }

            Global {
                id: glob
                objectName: "updatesGlobal"
                anchors { left: parent.left; right: parent.right }

                height: hidden ? 0 : units.gu(8)
                status: updates.status
                requireRestart: updates.haveSystemUpdate
                updatesCount: updates.updatesCount
                online: updates.online
                onStop: updates.cancelChecks()

                onPause: {
                    downloadHandler.pauseAll();
                    SystemImage.pauseDownload();
                    updates.status = SystemUpdate.StatusBatchModePaused;
                }
                onResume: {
                    downloadHandler.resumeAll();
                    SystemImage.downloadUpdate();
                    updates.status = SystemUpdate.StatusBatchMode;
                }
                onRequestInstall: {
                    if (requireRestart) {
                        var popup = PopupUtils.open(updatePrompt, null, {
                            havePowerForUpdate: updates.havePower
                        });
                        popup.requestSystemUpdate.connect(function () {
                            install();
                        });
                    } else {
                        postClickBatchHandler.target = clickModel;
                        install();
                    }
                }
                onInstall: updates.status = SystemUpdate.StatusBatchMode;
            }

            Rectangle {
                id: overlay
                objectName: "updatesOverlay"
                visible: false
                color: "white"
                width: parent.width
                height: units.gu(10)

                Label {
                    id: placeholder
                    objectName: "updatesOverlayText"
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    text: {
                        var s = updates.status;
                        if (!updates.online) {
                            return i18n.tr("Connect to the Internet to check for updates.");
                        } else if (s === SystemUpdate.StatusIdle && updates.updatesCount === 0) {
                            return i18n.tr("Software is up to date");
                        } else if (s === SystemUpdate.StatusServerError ||
                                   s === SystemUpdate.StatusNetworkError) {
                            return i18n.tr("The update server is not responding. Try again later.");
                        }
                        return "";
                    }
                }
            }

            SettingsItemTitle {
                id: updatesAvailableHeader
                text: i18n.tr("Updates available")
                visible: updatesCount > 1
            }

            Column {
                id: imageUpdate
                objectName: "updatesImageUpdate"
                anchors { left: parent.left; right: parent.right }
                visible: updates.haveSystemUpdate

                Repeater {
                    model: imageModel

                    delegate: ImageUpdateDelegate {
                        objectName: "updatesImageDelegate-" + index
                        anchors { left: parent.left; right: parent.right }
                        updateState: model.updateState
                        progress: model.progress
                        version: remoteVersion
                        size: model.size
                        changelog: model.changelog

                        onRetry: SystemImage.downloadUpdate();
                        onDownload: SystemImage.downloadUpdate();
                        onPause: SystemImage.pauseDownload();
                        onInstall: {
                            var popup = PopupUtils.open(updatePrompt, null, {
                                havePowerForUpdate: updates.havePower
                            });
                            popup.requestSystemUpdate.connect(SystemImage.applyUpdate);
                        }
                    }
                }
            }

            Column {
                id: clickUpdates
                objectName: "updatesClickUpdates"
                anchors { left: parent.left; right: parent.right }
                visible: clickModel.count > 0

                Repeater {
                    model: clickModel

                    delegate: ClickUpdateDelegate {
                        objectName: "updatesClickUpdate" + index
                        anchors { left: parent.left; right: parent.right }
                        updateState: model.updateState
                        progress: model.progress
                        version: remoteVersion
                        size: model.size
                        name: title
                        iconUrl: model.iconUrl
                        changelog: model.changelog

                        onInstall: downloadHandler.createDownload(model)
                        onDownload: downloadHandler.createDownload(model)

                        onPause: downloadHandler.pauseDownload(model)
                        onResume: downloadHandler.resumeDownload(model)

                        Connections {
                            target: glob
                            onInstall: install()
                        }
                    }
                }

            }

            NotAuthenticatedNotification {
                id: notauthNotification
                objectName: "updatesNoAuthNotif"
                visible: false
                anchors {
                    left: parent.left
                    right: parent.right
                }
                onRequestAuthentication: updates.requestAuthentication()
            }

            SettingsItemTitle {
                text: i18n.tr("Recent updates")
                visible: installed.visible
            }

            Column {
                id: installed
                objectName: "updatesInstalledUpdates"
                anchors { left: parent.left; right: parent.right }
                visible: installedModel.count > 0

                Repeater {

                    model: installedModel

                    delegate: UpdateDelegate {
                        objectName: "updatesInstalledUpdate-" + index
                        anchors { left: parent.left; right: parent.right }
                        version: remoteVersion
                        size: model.size
                        name: title
                        kind: model.kind
                        iconUrl: model.iconUrl
                        changelog: model.changelog
                        updateState: Update.StateInstalled
                        updatedAt: model.updatedAt

                        onLaunch: {
                            console.warn("%1_%2_%3".arg(identifier).arg(packageName).arg(remoteVersion));
                            /* The Application ID is the string
                            "$(click_package)_$(application)_$(version) */
                            clickManager.launch("%1_%2_%3".arg(identifier).arg(packageName).arg(remoteVersion));
                        }
                    }
                }
            }
        } // Column inside flickable.
    }

    Connections {
        target: clickManager
        function onCheckStop() {
            switch (updates.status) {
            case SystemUpdate.StatusCheckingClickUpdates:
                updates.status = SystemUpdate.StatusIdle; break;
            case SystemUpdate.StatusCheckingAllUpdates:
                updates.status = SystemUpdate.StatusCheckingSystemUpdates; break;
            }
        }
        onCheckFailed: onCheckStop()
        onCheckCompleted: onCheckStop()
        onCheckCanceled: onCheckStop()
        onNetworkError: updates.status = SystemUpdate.StatusNetworkError
        onServerError: updates.status = SystemUpdate.StatusServerError
        onCredentialError: console.warn('Credential error');
        function onCheckStart() {
            switch (updates.status) {
            case SystemUpdate.StatusIdle:
                updates.status = SystemUpdate.StatusCheckingClickUpdates; break;
            case SystemUpdate.StatusCheckingSystemUpdates:
                updates.status = SystemUpdate.StatusCheckingAllUpdates; break;
            }
        }
        onCheckStarted: onCheckStart()
    }

    Connections {
        target: SystemImage
        onUpdateAvailableStatus: {
            updates.haveSystemUpdate = isAvailable;

            switch (updates.status) {
            case SystemUpdate.StatusCheckingAllUpdates:
                updates.status = SystemUpdate.StatusCheckingClickUpdates; break;
            case SystemUpdate.StatusCheckingSystemUpdates:
                updates.status = SystemUpdate.StatusIdle; break;
            }
        }
    }

    Connections {
        id: postClickBatchHandler
        ignoreUnknownSignals: true
        target: null
        onCountChanged: {
            if (target.count === 0) {
                updates.status = SystemUpdate.StatusIdle;
                target = null;
            }
        }
    }

    Component.onCompleted: {
        if (SystemUpdate.isCheckRequired()) {
            checkClick();
            checkSystem();
            console.warn('do check');
        }
    }
    Component.onDestruction: cancelChecks()

    Component {
        id: updatePrompt

        ImageUpdatePrompt {}
    }
}
