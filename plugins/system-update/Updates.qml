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
import Ubuntu.DownloadManager 1.2
import Ubuntu.SystemSettings.Update 1.0

Item {
    id: updates
    property var clickUpdateManager
    property var clickUpdatesModel
    property var previousUpdatesModel
    property var downloadHandler

    property Flickable flickable: scrollWidget
    property bool havePower: false
    property bool online: NetworkingStatus.online
    property bool haveSystemUpdate: SystemImage.checkTarget()
    property bool authenticated: true
    property int status: SystemUpdate.StatusIdle
    property int updatesCount: {
        var count = 0;
        count += updates.haveSystemUpdate ? 1 : 0;
        count += clickUpdatesModel.count
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
        clickUpdateManager.check();
    }

    function cancelChecks() {
        clickUpdateManager.cancel();
        updates.status = SystemUpdate.StatusIdle;
    }

    signal requestAuthentication()
//    signal udmDownloadCreated(string packageName, int revision, int udmId)

    states: [
        State {
            name: "offline"
            PropertyChanges { target: overlay; visible: true }
            PropertyChanges { target: glob; hidden: true }
            PropertyChanges { target: imageUpdate; visible: false }
            PropertyChanges { target: clickUpdates; visible: false }
            when: !online
        },
        State {
            name: "error"
            PropertyChanges { target: overlay; visible: true }
            PropertyChanges { target: glob; hidden: true }
            PropertyChanges { target: imageUpdate; visible: false }
            PropertyChanges { target: clickUpdates; visible: false }
            when: status === SystemUpdate.StatusNetworkError ||
                  status === SystemUpdate.StatusServerError
        },
        State {
            name: "noAuth"
            when: !updates.authenticated
            PropertyChanges { target: notauthNotification; visible: true }
            PropertyChanges { target: noauthDivider; visible: (updates.haveSystemUpdate || !glob.hidden) }
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
        boundsBehavior: (contentHeight > parent.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        contentHeight: {
            var h = 0;
            h += glob.hidden ? 0 : glob.height;
            h += imageUpdate.visible ? imageUpdate.height : 0;
            h += clickUpdates.visible ? clickUpdates.height : 0;
            h += noauthDivider.visible ? noauthDivider.height : 0;
            h += notauthNotification.visible ? notauthNotification.height : 0;
            return h;
        }
        flickableDirection: Flickable.VerticalFlick

        Global {
            id: glob
            objectName: "updatesGlobal"
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(2)
            }

            height: hidden ? 0 : units.gu(6)
            status: updates.status
            requireRestart: updates.haveSystemUpdate
            updatesCount: updates.updatesCount
            online: updates.online
            onStop: updates.cancelChecks()
        }

        ImageUpdateDelegate {
            id: imageUpdate
            objectName: "updatesImageUpdate"
            visible: updates.haveSystemUpdate
            anchors {
                left: parent.left
                right: parent.right
                top: glob.bottom
            }
        }

        Column {
            id: clickUpdates
            objectName: "updatesClickUpdates"
            visible: !!clickUpdatesRepeater.model && clickUpdatesRepeater.model.count > 0
            anchors {
                left: parent.left
                right: parent.right
                top: updates.haveSystemUpdate ? imageUpdate.bottom : glob.bottom
            }
            height: childrenRect.height

            Repeater {
                id: clickUpdatesRepeater
                model: clickUpdatesModel
                height: childrenRect.height
                delegate: ClickUpdateDelegate {
                    objectName: "updatesClickUpdate" + index
                    anchors { left: clickUpdates.left; right: clickUpdates.right }
                    downloader: downloadHandler
                    updateModel: clickUpdatesModel
                    // command: model.command
                    // packageName: identifier
                    // revision: model.revision
                    // clickToken: token
                    // downloadUrl: model.downloadUrl
                    updateState: model.updateState
                    progress: model.progress
                    // downloadSha512: downloadHash
                    version: remoteVersion
                    size: model.size
                    name: title
                    iconUrl: model.iconUrl
                    changelog: model.changelog
                    // onPause: {
                    //     console.warn('onPause', getDownload(model.downloadId));
                    //     try {
                    //         getDownload(model.downloadId).pause();
                    //     } catch (e) {
                    //         clickUpdatesModel.setError(
                    //             model.downloadId, i18n.tr("Installation failed.")
                    //         );
                    //     }
                    // }
                    // onResume: {
                    //     console.warn('onResume', getDownload(model.downloadId));
                    //     try {
                    //         getDownload(model.downloadId).resume();
                    //     } catch (e) {
                    //         clickUpdatesModel.setError(
                    //             model.downloadId, i18n.tr("Installation failed.")
                    //         );
                    //     }
                    // }
                    // onInstall: {
                    //     console.warn("onInstall", identifier, model.revision);
                    //     if (model.downloadId) {
                    //         console.warn('already had download id');
                    //     } else {
                    //         createDownload(model);
                    //     }
                    // }
                    // onRetry: {
                    //     updateState = Update.StateUnavailable;
                    //     clickUpdateManager.check(model.identifier);
                    // }
                    // onPause: downloadHandler.pause(model.downloadId)

                    // Component.onCompleted: {
                    //     console.warn('onCompleted check on ', model.title);
                    //     var dl;
                    //     if (model.downloadId) {
                    //         dl = getDownload(model.downloadId);
                    //         if (dl && !dl._bound)
                    //             bindDownload(dl);
                    //     } else {
                    //         // if (model.automatic)
                    //             //createDownload(model);
                    //     }
                    // }
                }
            }

            Column {
                id: previous
                objectName: "updatesPreviousUpdates"
                visible: previousUpdatesRepeater.model && previousUpdatesRepeater.model.count > 0
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: childrenRect.height

                SettingsItemTitle {
                    text: i18n.tr("Previous Updates")
                }

                Repeater {
                    id: previousUpdatesRepeater
                    model: previousUpdatesModel
                    height: childrenRect.height
                    delegate: UpdateDelegate {
                        objectName: "updatesPreviousUpdate" + index
                        width: previous.width
                        version: remoteVersion
                        size: model.size
                        name: title
                        iconUrl: model.iconUrl
                        changelog: model.changelog
                        updateState: Update.StateInstalled
                    }
                }
            }
        }

        ListItems.ThinDivider {
            id: noauthDivider
            visible: false
            anchors {
                left: parent.left
                right: parent.left
                top: imageUpdate.visible ? imageUpdate.bottom : glob.bottom
            }
        }

        NotAuthenticatedNotification {
            id: notauthNotification
            objectName: "updatesNoAuthNotif"
            visible: false
            anchors {
                top: {
                    if (noauthDivider.visible)
                        return noauthDivider.bottom
                    else if (imageUpdate.visible)
                        return imageUpdate.bottom
                    else
                        return glob.bottom
                }
                topMargin: units.gu(5)
            }
            onRequestAuthentication: updates.requestAuthentication()
        }
    }

    Rectangle {
        id: overlay
        objectName: "updatesFullscreenMessage"
        visible: false
        color: "white"
        anchors.fill: parent

        Label {
            id: placeholder
            objectName: "updatesFullscreenMessageText"
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

    Connections {
        target: clickUpdateManager
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
            // bool isAvailable,
            // bool downloading,
            // QString &availableVersion,
            // int &updateSize,
            // QString &lastUpdateDate,
            // QString &errorReason)
            console.warn('onUpdateAvailableStatus', isAvailable, downloading, availableVersion, updateSize, lastUpdateDate, errorReason);
            updates.haveSystemUpdate = isAvailable;

            switch (updates.status) {
            case SystemUpdate.StatusCheckingAllUpdates:
                updates.status = SystemUpdate.StatusCheckingClickUpdates; break;
            case SystemUpdate.StatusCheckingSystemUpdates:
                updates.status = SystemUpdate.StatusIdle; break;
            }
        }
    }

    Component.onCompleted: {
        if (clickUpdateManager.isCheckRequired()) {
            checkClick();
            checkSystem();
            console.warn('do check');
        }
    }
    Component.onDestruction: cancelChecks()
}
