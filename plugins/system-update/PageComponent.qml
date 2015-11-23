/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2014 Canonical Ltd.
 *
 * Contact: Didier Roche <didier.roches@canonical.com>
 * Contact: Diego Sarmentero <diego.sarmentero@canonical.com>
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
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Components.Popups 1.3
import Ubuntu.OnlineAccounts.Client 0.1
import Ubuntu.SystemSettings.Update 1.0
import Ubuntu.Connectivity 1.0


ItemPage {
    id: root
    objectName: "systemUpdatesPage"

    title: installingImageUpdate.visible ? "" : i18n.tr("Updates")
    flickable: installingImageUpdate.visible ? null : scrollWidget

    property bool installAll: false
    property bool includeSystemUpdate: false
    property bool systemUpdateInProgress: false
    property int updatesAvailable: 0
    property bool isCharging: indicatorPower.deviceState === "charging"
    property bool batterySafeForUpdate: isCharging || chargeLevel > 25
    property var chargeLevel: indicatorPower.batteryLevel || 0
    property var notificationAction;
    property string errorDialogText: ""

    onUpdatesAvailableChanged: {
        if (updatesAvailable < 1 && root.state != "SEARCHING")
            root.state = "NOUPDATES";
    }

    QDBusActionGroup {
        id: indicatorPower
        busType: 1
        busName: "com.canonical.indicator.power"
        objectPath: "/com/canonical/indicator/power"
        property variant batteryLevel: action("battery-level").state
        property variant deviceState: action("device-state").state
        Component.onCompleted: start()
    }

    Connections {
        id: networkingStatus
        target: NetworkingStatus
        onOnlineChanged: {
            if (NetworkingStatus.online) {
                activity.running = true;
                root.state = "SEARCHING";
                UpdateManager.checkUpdates();
            } else {
                activity.running = false;
            }
        }
    }

    Setup {
        id: uoaConfig
        applicationId: "ubuntu-system-settings"
        providerId: "ubuntuone"

        onFinished: {
            credentialsNotification.visible = false;
            root.state = "SEARCHING";
            if (NetworkingStatus.online)
                UpdateManager.checkUpdates();
        }
    }

    Component {
         id: dialogInstallComponent
         Dialog {
             id: dialogueInstall
             title: i18n.tr("Update System")
             text: root.batterySafeForUpdate ? i18n.tr("The phone needs to restart to install the system update.") : i18n.tr("Connect the phone to power before installing the system update.")

             Button {
                 text: i18n.tr("Restart & Install")
                 visible: root.batterySafeForUpdate ? true : false
                 color: UbuntuColors.green
                 onClicked: {
                     installingImageUpdate.visible = true;
                     UpdateManager.applySystemUpdate();
                     PopupUtils.close(dialogueInstall);
                 }
             }
             Button {
                 text: i18n.tr("Cancel")
                 color: UbuntuColors.lightGrey
                 onClicked: {
                     updateList.currentIndex = 0;
                     var item = updateList.currentItem;
                     var modelItem = UpdateManager.model[0];
                     item.actionButton.text = i18n.tr("Update");
                     item.progressBar.opacity = 0;
                     modelItem.updateReady = true;
                     modelItem.selected = false;
                     root.systemUpdateInProgress = false;
                     PopupUtils.close(dialogueInstall);
                 }
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
                 color: UbuntuColors.green
                 onClicked: {
                     PopupUtils.close(dialogueError);
                 }
             }
         }
    }

    //states
    states: [
        State {
            name: "SEARCHING"
            PropertyChanges { target: installAllButton; visible: false}
            PropertyChanges { target: checkForUpdatesArea; visible: true}
            PropertyChanges { target: updateNotification; visible: false}
            PropertyChanges { target: activity; running: NetworkingStatus.online}
        },
        State {
            name: "NOUPDATES"
            PropertyChanges { target: updateNotification; text: i18n.tr("Software is up to date")}
            PropertyChanges { target: updateNotification; visible: true}
            PropertyChanges { target: updateList; visible: false}
            PropertyChanges { target: installAllButton; visible: false}
        },
        State {
            name: "SYSTEMUPDATEFAILED"
            PropertyChanges { target: installingImageUpdate; visible: false}
            PropertyChanges { target: installAllButton; visible: false}
            PropertyChanges { target: checkForUpdatesArea; visible: false}
            PropertyChanges { target: updateNotification; visible: false}
        },
        State {
            name: "UPDATE"
            PropertyChanges { target: updateList; visible: true}
            PropertyChanges { target: installAllButton; visible: root.updatesAvailable > 1}
            PropertyChanges { target: updateNotification; visible: false}
        }
    ]

    Connections {
        id: updateManager
        target: UpdateManager
        objectName: "updateManager"

        Component.onCompleted: {
            credentialsNotification.visible = false;
            root.state = "SEARCHING";
            if (NetworkingStatus.online)
                UpdateManager.checkUpdates();
        }

        onUpdateAvailableFound: {
            root.updatesAvailable = UpdateManager.model.length;
            if (root.updatesAvailable > 0)
                root.includeSystemUpdate = UpdateManager.model[0].systemUpdate
            root.state = "UPDATE";
            root.installAll = downloading;
        }

        onUpdatesNotFound: {
            if (!credentialsNotification.visible) {
                root.state = "NOUPDATES";
            }
        }

        onCheckFinished: {
            checkForUpdatesArea.visible = false;
        }

        onCredentialsNotFound: {
            credentialsNotification.visible = true;
        }

        onCredentialsDeleted: {
            credentialsNotification.visible = true;
        }

        onSystemUpdateDownloaded: {
            root.installAll = false;
        }

        onSystemUpdateFailed: {
            root.state = "SYSTEMUPDATEFAILED";
            root.errorDialogText = i18n.tr("Sorry, the system update failed.");
            PopupUtils.open(dialogErrorComponent);
        }

        onUpdateProcessFailed: {
            root.state = "SYSTEMUPDATEFAILED";
            root.errorDialogText = i18n.tr("Sorry, the system update failed.");
            PopupUtils.open(dialogErrorComponent);
        }

        onServerError: {
            activity.running = false;
        }

        onNetworkError: {
            activity.running = false;
        }

        onRebooting: {
            installingImageUpdate.message = i18n.tr("Restarting…");
        }
    }
    Flickable {
        id: scrollWidget

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: configuration.top

        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        clip: true
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: columnId
            anchors {
                left: parent.left
                right: parent.right
            }
            height: childrenRect.height

            ListItems.Base {
                id: checkForUpdatesArea
                objectName: "checkForUpdatesArea"
                visible: false

                ActivityIndicator {
                    id: activity
                    running: checkForUpdatesArea.visible
                    visible: activity.running
                    anchors {
                        left: parent.left
                        top: parent.top
                    }
                    height: parent.height
                }

                Label {
                    text: activity.running ? i18n.tr("Checking for updates…") : i18n.tr("Connect to the Internet to check for updates")
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.Wrap
                    anchors {
                        left: activity.running ? activity.right : parent.left
                        top: parent.top
                        right: parent.right
                        rightMargin: units.gu(2)
                        leftMargin: units.gu(2)
                    }
                    height: parent.height
                }
            }

            SettingsListItems.SingleValue {
                height: installAllButton.visible ? units.gu(8) : units.gu(2)
                highlightWhenPressed: false
                text: includeSystemUpdate ?
                    i18n.tr("%1 update available", "%1 updates available", root.updatesAvailable).arg(root.updatesAvailable) :
                    i18n.tr("%1 update available", "%1 updates available", root.updatesAvailable).arg(root.updatesAvailable)
                Button {
                    id: installAllButton
                    objectName: "installAllButton"
                    SlotsLayout.position: SlotsLayout.Trailing
                    color: UbuntuColors.green
                    text: root.installAll ? i18n.tr("Pause all") : i18n.tr("Update all")
                    onClicked: {
                        for (var i=0; i < updateList.count; i++) {
                            updateList.currentIndex = i;
                            var item = updateList.currentItem;
                            var modelItem = UpdateManager.model[i];
                            if (item.installing || item.installed)
                                continue;
                            console.warn("AllClicked: " + modelItem.updateState + " " + modelItem.updateReady + " " +  modelItem.selected);
                            if (item.retry) {
                                item.retry = false;
                                UpdateManager.retryDownload(modelItem.packageName);
                                continue;
                            }
                            if (root.installAll && !modelItem.updateReady && modelItem.selected) {
                                item.pause();
                                continue;
                            }
                            console.warn("Past pause");
                            if (!root.installAll && !modelItem.updateReady && modelItem.selected) {
                                item.resume();
                                continue;
                            }
                            console.warn("Past resume");
                            if (!root.installAll && !modelItem.updateState && !modelItem.updateReady && !modelItem.selected) {
                                item.start();
                                continue;
                            }
                            console.warn("Past start");
                        }
                        root.installAll = !root.installAll;
                    }
                }
            }

            ListView {
                id: updateList
                objectName: "updateList"
                anchors {
                    left: parent.left
                    right: parent.right
                }
                model: UpdateManager.model
                height: childrenRect.height
                interactive: false
                spacing: 0

                delegate: UpdateItem {
                    id: updateItem
                    updateData: modelData
                    tracker: DownloadTracker {
                        id: tracker
                        objectName: "tracker"
                        packageName: updateData.packageName
                        clickToken: updateData.clickToken
                        download: updateData.downloadUrl
                        downloadSha512: updateData.downloadSha512

                        onFinished: {
                            updateItem.progress.visible = false;
                            updateItem.buttonAppUpdate.visible = false;
                            updateItem.installed = true;
                            updateItem.installing = false;
                            root.updatesAvailable -= 1;
                            updateData.updateRequired = false;
                            UpdateManager.updateClickScope();
                        }

                        onProcessing: {
                            console.warn("onProcessing: " + updateData.packageName + " " + path);
                            updateItem.buttonAppUpdate.enabled = false;
                            updateItem.installing = true;
                            updateData.updateState = false;
                        }

                        onStarted: {
                            console.warn("onStarted: " + updateData.packageName + " " + success);
                            if (success)
                                updateData.updateState = true;
                            else
                                updateData.updateState = false;
                        }

                        onPaused: {
                            console.warn("onPaused: " + updateData.packageName + " " + success);
                            if (success)
                                updateData.updateState = false;
                            else
                                updateData.updateState = true;
                        }

                        onResumed: {
                            console.warn("onResumed: " + updateData.packageName + " " + success);
                            if (success)
                                updateData.updateState = true;
                            else
                                updateData.updateState = false;
                        }

                        onCanceled: {
                            console.warn("onCanceled: " + updateData.packageName + " " + success);
                            if (success) {
                                updateData.updateState = false;
                                updateData.selected = false;
                            }
                        }

                        onErrorFound: {
                            console.warn("onErrorFound: " + updateData.packageName + " " + error);
                            updateData.updateState = false;
                            updateItem.retry = true;
                            updateItem.installing = false;
                        }
                    }
                    
                    function pause () {
                        console.warn("PAUSE: " + updateData.packageName);
                        if (updateData.systemUpdate)
                            return UpdateManager.pauseDownload(updateData.packageName);
                        updateData.updateState = false;
                        tracker.pause();
                    }

                    function resume () {
                        console.warn("RESUME: " + updateData.packageName);
                        if (updateData.systemUpdate)
                            return UpdateManager.startDownload(updateData.packageName);
                        updateData.updateState = true;
                        tracker.resume();
                    }

                    function start () {
                        console.warn("START: " + updateData.packageName);
                        updateData.selected = true;
                        updateData.updateState = true;
                        UpdateManager.startDownload(updateData.packageName);
                    }
                }
            }

            Column {
                id: credentialsNotification
                objectName: "credentialsNotification"

                visible: false

                spacing: units.gu(2)
                anchors {
                    left: parent.left
                    right: parent.right
                }
                ListItems.ThinDivider {}

                Label {
                    text: i18n.tr("Sign in to Ubuntu One to receive updates for apps.")
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.Wrap
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                }
                Button {
                    text: i18n.tr("Sign In…")
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: units.gu(2)
                        rightMargin: units.gu(2)
                    }
                    color: UbuntuColors.lightGrey
                    onClicked: uoaConfig.exec()
                }

            }
        }
    }

    Rectangle {
        id: updateNotification
        objectName: "updateNotification"
        anchors {
            bottom: configuration.top
            left: parent.left
            right: parent.right
            top: parent.top
        }
        visible: false
        property string text: ""

        color: "transparent"

        Label {
            anchors.centerIn: updateNotification
            text: updateNotification.text
            width: updateNotification.width
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
        }
    }

    Rectangle {
        id: installingImageUpdate
        objectName: "installingImageUpdate"
        anchors.fill: root
        visible: false
        z: 10
        color: "#221e1c"
        property string message: i18n.tr("Installing update…")

        Column {
            anchors.centerIn: parent
            spacing: units.gu(2)

            Image {
                source: Qt.resolvedUrl("file:///usr/share/icons/suru/places/scalable/distributor-logo.svg")
                anchors.horizontalCenter: parent.horizontalCenter
                height: width
                width: 96
                NumberAnimation on rotation {
                    from: 0
                    to: 360
                    running: installingImageUpdate.visible == true
                    loops: Animation.Infinite
                    duration: 2000
                }
            }

            ProgressBar {
                indeterminate: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                text: installingImageUpdate.message
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Column {
        id: configuration

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        ListItems.ThinDivider {}
        SettingsListItems.SingleValueProgression {
            objectName: "configuration"
            text: i18n.tr("Auto download")
            value: {
                if (UpdateManager.downloadMode === 0)
                    return i18n.tr("Never")
                else if (UpdateManager.downloadMode === 1)
                    return i18n.tr("On wi-fi")
                else if (UpdateManager.downloadMode === 2)
                    return i18n.tr("Always")
            }
            onClicked: pageStack.push(Qt.resolvedUrl("Configuration.qml"))
        }
    }
}
