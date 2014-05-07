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

import QtQuick 2.0
import QtSystemInfo 5.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import Ubuntu.SystemSettings.Update 1.0


ItemPage {
    id: root
    objectName: "systemUpdatesPage"

    title: i18n.tr("Updates")

    property bool installAll: false
    property int updatesAvailable: 0

    DeviceInfo {
        id: deviceInfo
    }

    Component {
         id: dialogInstallComponent
         Dialog {
             id: dialogueInstall
             title: i18n.tr("Update System")
             text: i18n.tr("The phone needs to restart to install the system update.")

             Button {
                 text: i18n.tr("Install & Restart")
                 color: UbuntuColors.orange
                 onClicked: {
                     installingImageUpdate.visible = true;
                     updateManager.applySystemUpdate();
                     PopupUtils.close(dialogueInstall);
                 }
             }
             Button {
                 text: i18n.tr("Not Now")
                 color: UbuntuColors.warmGrey
                 onClicked: {
                     updateList.currentIndex = 0;
                     var item = updateList.currentItem;
                     var modelItem = updateManager.model[0];
                     item.actionButton.text = i18n.tr("Install");
                     modelItem.updateReady = true;
                     PopupUtils.close(dialogueInstall);
                 }
             }
         }
    }

    //states
    states: [
        State {
            name: "SEARCHING"
            PropertyChanges { target: notification; visible: false}
            PropertyChanges { target: installAllButton; visible: false}
            PropertyChanges { target: checkForUpdatesArea; visible: true}
            PropertyChanges { target: updateNotification; visible: false}
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
            PropertyChanges { target: notification; text: i18n.tr("System update has failed.")}
            PropertyChanges { target: notification; onClicked: undefined }
            PropertyChanges { target: notification; progression: false}
            PropertyChanges { target: notification; visible: true}
            PropertyChanges { target: installingImageUpdate; visible: false}
            PropertyChanges { target: installAllButton; visible: false}
            PropertyChanges { target: checkForUpdatesArea; visible: false}
            PropertyChanges { target: updateNotification; visible: false}
        },
        State {
            name: "UPDATE"
            PropertyChanges { target: notification; visible: false}
            PropertyChanges { target: updateList; visible: true}
            PropertyChanges { target: installAllButton; visible: true}
            PropertyChanges { target: updateNotification; visible: false}
        },
        State {
            name: "NOCREDENTIALS"
            PropertyChanges { target: notification; text: i18n.tr("Please log into your Ubuntu One account.")}
            PropertyChanges { target: notification; onClicked: root.open_online_accounts() }
            PropertyChanges { target: notification; progression: true}
            PropertyChanges { target: notification; visible: true}
            PropertyChanges { target: updateNotification; text: i18n.tr("Credentials not found")}
            PropertyChanges { target: updateNotification; visible: true}
            PropertyChanges { target: installAllButton; visible: false}
        }
    ]

    function open_online_accounts() {
        Qt.openUrlExternally("settings:///system/online-accounts");
    }

    UpdateManager {
        id: updateManager
        objectName: "updateManager"

        Component.onCompleted: {
            root.state = "SEARCHING";
            updateManager.checkUpdates();
        }

        onUpdateAvailableFound: {
            if (updateManager.model.length > 0) {
                root.updatesAvailable = updateManager.model.length;
                root.state = "UPDATE";
                root.installAll = downloading
            } else {
                root.state = "NOUPDATES";
            }
        }

        onUpdatesNotFound: {
            if (root.state != "NOCREDENTIALS") {
                root.state = "NOUPDATES";
            }
        }

        onCheckFinished: {
            checkForUpdatesArea.visible = false;
        }

        onCredentialsNotFound: {
            root.state = "NOCREDENTIALS";
        }

        onSystemUpdateDownloaded: {
            root.updatesAvailable -= 1;
            PopupUtils.open(dialogInstallComponent);
        }

        onSystemUpdateFailed: {
            root.state = "SYSTEMUPDATEFAILED";
            if (lastReason) {
                notification.text = lastReason;
            }
        }

        onUpdateProcessFailed: {
            root.state = "SYSTEMUPDATEFAILED";
            if (message) {
                notification.text = message;
            } else {
                notification.text = i18n.tr("System update failed.");
            }
        }
    }

    Item {
        id: checkForUpdatesArea
        objectName: "checkForUpdatesArea"
        visible: false
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            topMargin: units.gu(2)
            leftMargin: units.gu(4)
            rightMargin: units.gu(4)
        }
        height: installAllButton.height

        ActivityIndicator {
            id: activity
            running: checkForUpdatesArea.visible
            visible: activity.running
            anchors {
                left: parent.left
                top: parent.top
                topMargin: units.gu(1)
            }
        }

        Label {
            text: i18n.tr("Checking for updates…")
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            anchors {
                left: activity.right
                top: parent.top
                right: checkForUpdatesArea.right
                bottom: parent.bottom
                leftMargin: units.gu(2)
                rightMargin: units.gu(2)
            }
        }
    }

    Button {
        id: installAllButton
        objectName: "installAllButton"

        property string primaryText: i18n.tr("Install %1 update", "Install %1 updates", root.updatesAvailable).arg(root.updatesAvailable)
        property string secondaryText: i18n.tr("Pause All")
        text: root.installAll ? secondaryText : primaryText
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            topMargin: units.gu(2)
            leftMargin: units.gu(4)
            rightMargin: units.gu(4)
        }
        visible: false

        color: UbuntuColors.orange
        onClicked: {
            root.installAll = !root.installAll;
            for (var i=0; i < updateList.count; i++) {
                updateList.currentIndex = i;
                var item = updateList.currentItem;
                var modelItem = updateManager.model[i];
                if (modelItem.updateState != root.installAll && !modelItem.updateReady) {
                    item.actionButton.clicked();
                }
            }
        }
        opacity: root.updatesAvailable > 0 ? 1 : 0

        Behavior on opacity { PropertyAnimation { duration: UbuntuAnimation.SlowDuration
                easing: UbuntuAnimation.StandardEasing } }
    }

    ListView {
        id: updateList
        objectName: "updateList"
        visible: false
        anchors {
            left: parent.left
            right: parent.right
            top: installAllButton.bottom
            bottom: notification.visible ? notification.top : configuration.top
            margins: units.gu(2)
            bottomMargin: 0
        }
        model: updateManager.model
        clip: true
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > (root.height - checkForUpdatesArea.height)) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        delegate: ListItem.Subtitled {
            id: listItem
            iconSource: Qt.resolvedUrl(modelData.iconUrl)
            iconFrame: false
            height: modelData.selected ? units.gu(14) : units.gu(8)

            property alias actionButton: buttonAppUpdate

            Rectangle {
                id: textArea
                objectName: "textArea"
                color: "transparent"
                anchors.fill: parent
                anchors.topMargin: units.gu(1)

                property string message: modelData.error
                property bool retry: false

                onMessageChanged: {
                    if(message.length > 0) {
                        labelVersion.text = message;
                        buttonAppUpdate.text = i18n.tr("Retry");
                        modelData.updateState = false;
                        modelData.selected = false;
                        textArea.retry = true;
                    }
                }

                Button {
                    id: buttonAppUpdate
                    objectName: "buttonAppUpdate"
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.topMargin: units.gu(1)
                    anchors.rightMargin: units.gu(1)
                    height: labelTitle.height

                    property string actionText: modelData.systemUpdate ? i18n.tr("Download") : i18n.tr("Update")
                    property string primaryText: modelData.selected ? i18n.tr("Resume") : actionText
                    property string secondaryText: i18n.tr("Pause")

                    text: modelData.updateState ? secondaryText : primaryText

                    onClicked: {
                        if (textArea.retry) {
                            textArea.retry = false;
                            updateManager.retryDownload(modelData.packageName);
                        } else if (modelData.updateReady) {
                            updateManager.applySystemUpdate();
                            installingImageUpdate.visible = true;
                        } else if (modelData.updateState) {
                            if (modelData.systemUpdate) {
                                updateManager.pauseDownload(modelData.packageName);
                            } else {
                                modelData.updateState = false;
                                tracker.pause();
                            }
                        } else {
                            if (!modelData.selected || modelData.systemUpdate) {
                                modelData.selected = true;
                                updateManager.startDownload(modelData.packageName);
                            } else {
                                modelData.updateState = true;
                                tracker.resume();
                            }
                        }
                    }
                }

                Label {
                    id: labelSize
                    objectName: "labelSize"
                    text: convert_bytes_to_size(modelData.binaryFilesize)
                    anchors.bottom: labelVersion.bottom
                    anchors.right: parent.right
                    anchors.rightMargin: units.gu(1)
                    visible: !modelData.selected
                }

                Label {
                    id: labelTitle
                    objectName: "labelTitle"
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: buttonAppUpdate.left
                        topMargin: units.gu(1)
                        rightMargin: units.gu(1)
                    }
                    height: units.gu(3)
                    text: modelData.title
                    font.bold: true
                    elide: buttonAppUpdate.visible ? Text.ElideRight : Text.ElideNone
                }

                Label {
                    id: labelUpdateStatus
                    objectName: "labelUpdateStatus"
                    text: i18n.tr("Installing")
                    anchors.top: labelTitle.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    opacity: modelData.selected ? 1 : 0
                    anchors.bottomMargin: units.gu(1)

                    Behavior on opacity { PropertyAnimation { duration: UbuntuAnimation.SleepyDuration } }
                }

                ProgressBar {
                    id: progress
                    objectName: "progress"
                    height: units.gu(2)
                    anchors.left: parent.left
                    anchors.top: labelUpdateStatus.bottom
                    anchors.topMargin: units.gu(1)
                    anchors.right: parent.right
                    opacity: modelData.selected ? 1 : 0
                    value: modelData.systemUpdate ? modelData.downloadProgress : tracker.progress
                    minimumValue: 0
                    maximumValue: 100

                    DownloadTracker {
                        id: tracker
                        objectName: "tracker"
                        packageName: modelData.packageName
                        clickToken: modelData.clickToken
                        download: modelData.downloadUrl

                        onFinished: {
                            progress.visible = false;
                            buttonAppUpdate.visible = false;
                            textArea.message = i18n.tr("Installed");
                            root.updatesAvailable -= 1;
                        }

                        onErrorFound: {
                            modelData.updateState = false;
                            textArea.message = error;
                        }
                    }

                    Behavior on opacity { PropertyAnimation { duration: UbuntuAnimation.SleepyDuration } }
                }

                Label {
                    id: labelVersion
                    objectName: "labelVersion"
                    anchors {
                        left: parent.left
                        right: buttonAppUpdate.right
                        top: (!progress.visible || progress.opacity == 0) ? labelTitle.bottom : progress.bottom
                        topMargin: (!progress.visible || progress.opacity == 0) ? 0 : units.gu(1)
                        bottom: parent.bottom
                        bottomMargin: units.gu(1)
                    }
                    text: modelData.remoteVersion ? i18n.tr("Version: ") + modelData.remoteVersion : ""
                    color: "black"
                    elide: Text.ElideRight
                }
            }
        }
    }

    ListItem.Standard {
        id: notification
        objectName: "notification"
        visible: false
        anchors.bottom: configuration.top
    }

    ListItem.SingleValue {
        id: configuration
        objectName: "configuration"
        anchors.bottom: parent.bottom
        text: i18n.tr("Auto download")
        value: {
            if (updateManager.downloadMode === 0)
                return i18n.tr("Never")
            else if (updateManager.downloadMode === 1)
                return i18n.tr("On wi-fi")
            else if (updateManager.downloadMode === 2)
                return i18n.tr("Always")
        }
       progression: true
       onClicked: pageStack.push(Qt.resolvedUrl("Configuration.qml"), {updateManager: updateManager})
    }

    Rectangle {
        id: updateNotification
        objectName: "updateNotification"
        anchors {
            left: parent.left
            right: parent.right
            top: installAllButton.bottom
            bottom: notification.visible ? notification.top : configuration.top
            margins: units.gu(2)
            bottomMargin: 0
        }
        visible: false
        property string text: ""

        color: "transparent"

        Column {
            anchors.centerIn: parent

            Label {
                text: updateNotification.text
                anchors.horizontalCenter: parent.horizontalCenter
                fontSize: "large"
            }
        }
    }

    Rectangle {
        id: installingImageUpdate
        objectName: "installingImageUpdate"
        anchors.fill: parent
        visible: false

        color: "#221e1c"

        Column {
            anchors.centerIn: parent
            spacing: units.gu(2)

            Image {
                source: Qt.resolvedUrl("file:///usr/share/ubuntu/settings/system/icons/distributor-logo.png")
                anchors.horizontalCenter: parent.horizontalCenter
            }

            ProgressBar {
                indeterminate: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                text: i18n.tr("Installing update…")
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    function convert_bytes_to_size(bytes) {
        var SIZE_IN_GIB = 1024.0 * 1024.0 * 1024.0;
        var SIZE_IN_MIB = 1024.0 * 1024.0;
        var SIZE_IN_KIB = 1024.0;

        var result = "";
        var size = 0;
        if (bytes < SIZE_IN_KIB) {
            result = bytes + i18n.tr(" bytes");
        } else if (bytes < SIZE_IN_MIB) {
            size = (bytes / SIZE_IN_KIB).toFixed(1);
            result = size + i18n.tr(" KiB");
        } else if (bytes < SIZE_IN_GIB) {
            size = (bytes / SIZE_IN_MIB).toFixed(1);
            result = size + i18n.tr(" MiB");
        } else {
            size = (bytes / SIZE_IN_GIB).toFixed(1);
            result = size + i18n.tr(" GiB");
        }

        return result;
    }
}
