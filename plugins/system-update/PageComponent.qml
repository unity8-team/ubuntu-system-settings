/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
 *
 * Contact: Didier Roche <didier.roches@canonical.com>
 *          Diego Sarmentero <diego.sarmentero@canonical.com>
 *          Jonas G. Drange <jonas.drange@canonical.com>
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

import QMenuModel 0.1
import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Connectivity 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.Components.Popups 1.3
import Ubuntu.OnlineAccounts.Client 0.1
import Ubuntu.SystemSettings.Update 1.0

ItemPage {
    id: root
    objectName: "systemUpdatesPage"

    header: PageHeader {
        title: root.title
        flickable: scrollWidget
    }

    QDBusActionGroup {
        id: indicatorPower
        busType: 1
        busName: "com.canonical.indicator.power"
        objectPath: "/com/canonical/indicator/power"
        property var batteryLevel: action("battery-level").state || 0
        property var deviceState: action("device-state").state
        Component.onCompleted: start()
    }

    property bool batchMode: false
    property bool havePower: (indicatorPower.deviceState === "charging") ||
                             (indicatorPower.batteryLevel > 25)
    property bool online: NetworkingStatus.online
    property bool authenticated: clickManager.authenticated
    property int status: {
        var c = clickManager.checkingForUpdates;
        var s = SystemImage.checkingForUpdates;
        if (c && s) {
            return SystemUpdate.StatusCheckingAllUpdates;
        } else if (c && !s) {
            return SystemUpdate.StatusCheckingClickUpdates;
        } else if (!c && s) {
            return SystemUpdate.StatusCheckingSystemUpdates;
        } else {
            return SystemUpdate.StatusIdle;
        }
    }
    property int updatesCount: {
        var count = 0;
        if (authenticated) {
            count += clickRepeater.count;
        }
        count += imageRepeater.count;
        return count;
    }

    Component.onCompleted: {
        if (SystemUpdate.isCheckRequired()) {
            clickManager.check();
            SystemImage.checkForUpdate();
        }
    }

    function cancelChecks() {
        clickManager.cancel();
        imageHandler.cancel();
    }

    ClickManager {
        id: clickManager
        onCheckCompleted: SystemUpdate.checkCompleted()

        // These states are final and thus break the binding.
        onNetworkError: status = SystemUpdate.StatusNetworkError
        onServerError: status = SystemUpdate.StatusServerError
    }

    Setup {
        id: uoaConfig
        objectName: "uoaConfig"
        applicationId: "ubuntu-system-settings"
        providerId: "ubuntuone"

        onFinished: {
            if (reply.errorName)
                console.warn('Online Accounts failed:', reply.errorName);
            else
                clickManager.check();
        }
    }

    DownloadHandler {
        id: downloadHandler
        updateModel: SystemUpdate.model
    }

    SystemImageHandler {
        id: imageHandler
        updateModel: SystemUpdate.model
    }

    Connections {
        target: SystemImage
        onUpdateAvailableStatus: SystemUpdate.checkCompleted()
    }

    Flickable {
        id: scrollWidget
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: configuration.top
        }
        clip: true
        contentHeight: content.height
        boundsBehavior: (contentHeight > parent.height) ?
                        Flickable.DragAndOvershootBounds :
                        Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: content
            anchors { left: parent.left; right: parent.right }

            Global {
                id: glob
                objectName: "global"
                anchors { left: parent.left; right: parent.right }

                height: hidden ? 0 : units.gu(8)
                clip: true
                status: root.status
                batchMode: root.batchMode
                requireRestart: imageRepeater.count > 0
                updatesCount: root.updatesCount
                online: root.online
                onStop: root.cancelChecks()

                onRequestInstall: {
                    if (requireRestart) {
                        var popup = PopupUtils.open(
                            Qt.resolvedUrl("ImageUpdatePrompt.qml"), null, {
                                havePowerForUpdate: root.havePower
                            }
                        );
                        popup.requestSystemUpdate.connect(function () {
                            install();
                        });
                    } else {
                        install();
                    }
                }
                onInstall: {
                    root.batchMode = true
                    if (requireRestart) {
                        postAllBatchHandler.target = root;
                    } else {
                        postClickBatchHandler.target = root;
                    }
                }
            }

            Rectangle {
                id: overlay
                objectName: "overlay"
                visible: placeholder.text
                color: theme.palette.normal.background
                width: parent.width
                height: units.gu(10)

                Label {
                    id: placeholder
                    objectName: "overlayText"
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    text: {
                        var s = root.status;
                        if (!root.online) {
                            return i18n.tr("Connect to the Internet to check for updates.");
                        } else if (s === SystemUpdate.StatusIdle && updatesCount === 0) {
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
                id: imageUpdateCol
                objectName: "updatesImageUpdate"
                anchors { left: parent.left; right: parent.right }
                visible: imageRepeater.count > 0

                Repeater {
                    id: imageRepeater
                    model: SystemUpdate.imageUpdates

                    delegate: ImageUpdateDelegate {
                        objectName: "updatesImageDelegate-" + index
                        width: imageUpdateCol.width
                        updateState: model.updateState
                        progress: model.progress
                        version: remoteVersion
                        size: model.size
                        changelog: model.changelog
                        error: model.error
                        kind: model.kind
                        iconUrl: model.iconUrl
                        name: title

                        onRetry: SystemImage.downloadUpdate();
                        onDownload: SystemImage.downloadUpdate();
                        onPause: imageHandler.pause();
                        onInstall: {
                            var popup = PopupUtils.open(
                                Qt.resolvedUrl("ImageUpdatePrompt.qml"), null, {
                                    havePowerForUpdate: root.havePower
                                }
                            );
                            popup.requestSystemUpdate.connect(SystemImage.applyUpdate);
                        }
                    }
                }
            }

            Column {
                id: clickUpdatesCol
                objectName: "updatesClickUpdates"
                anchors { left: parent.left; right: parent.right }
                visible: clickRepeater.count > 0

                Repeater {
                    id: clickRepeater
                    model: SystemUpdate.clickUpdates

                    delegate: ClickUpdateDelegate {
                        objectName: "updatesClickUpdate" + index
                        width: clickUpdatesCol.width
                        updateState: model.updateState
                        progress: model.progress
                        version: remoteVersion
                        size: model.size
                        name: title
                        iconUrl: model.iconUrl
                        kind: model.kind
                        changelog: model.changelog
                        error: model.error
                        automatic: model.automatic

                        onInstall: downloadHandler.createDownload(model);
                        onPause: downloadHandler.pauseDownload(model)
                        onResume: downloadHandler.resumeDownload(model)
                        onRetry: clickManager.retry(identifier, revision)

                        onAutomaticChanged: {
                            if (automatic && !model.downloadId) {
                                install();
                            }
                        }

                        Connections {
                            target: glob
                            onInstall: install()
                        }

                        /* If we a downloadId, we expect UDM to restore it
                        after some time. Workaround for lp:1603770. */
                        Timer {
                            id: downloadTimeout
                            interval: 30000
                            running: true
                            onTriggered: {
                                if (model.downloadId) {
                                    downloadHandler.assertDownloadExist(model);
                                }
                            }
                        }
                    }
                }
            }

            NotAuthenticatedNotification {
                id: notauthNotification
                objectName: "noAuthenticationNotification"
                visible: !authenticated
                anchors {
                    left: parent.left
                    right: parent.right
                }
                onRequestAuthentication: uoaConfig.exec()
            }

            SettingsItemTitle {
                text: i18n.tr("Recent updates")
                visible: installed.visible
            }

            Column {
                id: installed
                objectName: "installedUpdates"
                anchors { left: parent.left; right: parent.right }
                visible: installedRepeater.count > 0

                Repeater {
                    id: installedRepeater
                    model: SystemUpdate.installedUpdates

                    delegate: UpdateDelegate {
                        objectName: "installedUpdate-" + index
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
                            /* The Application ID is the string
                            "$(click_package)_$(application)_$(version) */
                            clickManager.launch("%1_%2_%3".arg(identifier)
                                                          .arg(packageName)
                                                          .arg(remoteVersion));
                        }
                    }
                }
            }
        } // Column inside flickable.
    } // Flickable

    Column {
        id: configuration

        height: childrenRect.height

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        ListItem.ThinDivider {}

        ListItem.SingleValue {
            objectName: "configuration"
            text: i18n.tr("Auto download")
            value: {
                if (SystemImage.downloadMode === 0)
                    return i18n.tr("Never")
                else if (SystemImage.downloadMode === 1)
                    return i18n.tr("On wi-fi")
                else if (SystemImage.downloadMode === 2)
                    return i18n.tr("Always")
                else
                    return i18n.tr("Unknown")
            }
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("Configuration.qml"))
        }
    }

    Connections {
        id: postClickBatchHandler
        ignoreUnknownSignals: true
        target: null
        onUpdatesCountChanged: {
            if (target.updatesCount === 0) {
                root.batchMode = false;
                target = null;
            }
        }
    }

    Connections {
        id: postAllBatchHandler
        ignoreUnknownSignals: true
        target: null
        onUpdatesCountChanged: {
            if (target.updatesCount === 1) {
                SystemImage.updateDownloaded.connect(function () {
                    SystemImage.applyUpdate();
                });
                imageHandler.download();
            }
        }
    }

    Component {
         id: dialogErrorComponent
         Dialog {
             id: dialogueError
             title: i18n.tr("Installation failed")
             text: root.errorDialogText

             Button {
                 text: i18n.tr("OK")
                 color: UbuntuColors.orange
                 onClicked: {
                     PopupUtils.close(dialogueError);
                 }
             }
         }
    }
}
