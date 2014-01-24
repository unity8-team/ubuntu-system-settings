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
import "js/utils.js" as Utils


ItemPage {
    id: root

    title: i18n.tr("Updates")

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
        },
        State {
            name: "NOUPDATES"
            PropertyChanges { target: updatedNotification; text: i18n.tr("Software is up to date")}
            PropertyChanges { target: updatedNotification; visible: true}
            PropertyChanges { target: installAllButton; visible: false}
            PropertyChanges { target: checkForUpdatesArea; visible: false}
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
            PropertyChanges { target: updatedNotification; visible: false}
        },
        State {
            name: "UPDATE"
            PropertyChanges { target: notification; visible: false}
            PropertyChanges { target: installAllButton; visible: true}
            PropertyChanges { target: checkForUpdatesArea; visible: false}
            PropertyChanges { target: updatedNotification; visible: false}
        }
    ]

    UpdateManager {
        id: updateManager
        objectName: "updateManager"

        Component.onCompleted: {
            root.state = "SEARCHING";
            updateManager.checkUpdates();
        }

        onUpdateAvailableFound: {
            if (updateManager.model.length > 0) {
                root.state = "UPDATE";
            } else {
                root.state = "NOUPDATES";
            }
        }

        onUpdatesNotFound: {
            root.state = "NOUPDATES";
        }

        onSystemUpdateDownloaded: {
            PopupUtils.open(dialogInstallComponent);
        }

        onSystemUpdateFailed: {
            root.state = "SYSTEMUPDATEFAILED";
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
            text: i18n.tr("Checking for updates...")
            fontSize: "large"
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            anchors {
                left: activity.right
                top: parent.top
                right: checkForUpdatesArea.right
                leftMargin: units.gu(2)
                topMargin: units.gu(1)
                rightMargin: units.gu(2)
            }
        }
    }

    Button {
        id: installAllButton
        objectName: "installAllButton"
        text: i18n.tr("Install %1 Update", "Install %1 Updates", updateManager.model.length).arg(updateManager.model.length)
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
            for (var i=0; i < updateList.count; i++) {
                updateList.currentIndex = i;
                var item = updateList.currentItem;
                item.actionButton.clicked();
            }
        }
    }

    ListView {
        id: updateList
        objectName: "updateList"
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

        delegate: ListItem.Subtitled {
            id: listItem
            icon: Qt.resolvedUrl(modelData.iconUrl)
            iconFrame: false
            height: units.gu(9)

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

                Column {
                    id: colRight
                    objectName: "colRight"
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.topMargin: units.gu(1)

                    Button {
                        id: buttonAppUpdate

                        property string primaryText: modelData.systemUpdate ? i18n.tr("Download") : i18n.tr("Update")
                        property string secondaryText: i18n.tr("Pause")

                        text: modelData.updateState ? secondaryText : primaryText
                        height: textArea.height / 2

                        onClicked: {
                            if (textArea.retry) {
                                textArea.retry = false;
                                updateManager.retryDownload(modelData.packageName);
                            } else if (modelData.updateReady) {
                                updateManager.applySystemUpdate();
                                installingImageUpdate.visible = true;
                            } else if (modelData.updateState) {
                                updateManager.pauseDownload(modelData.packageName);
                            } else {
                                modelData.selected = true;
                                buttonAppUpdate.primaryText = i18n.tr("Resume");
                                updateManager.startDownload(modelData.packageName);
                            }
                        }
                    }
                    Label {
                        text: Utils.convert_bytes_to_size(modelData.binaryFilesize)
                        height: textArea.height / 2
                        anchors.right: parent.right
                    }
                }

                Column {
                    id: colLeft
                    objectName: "colLeft"
                    anchors {
                        left: parent.left
                        bottom: parent.bottom
                        top: parent.top
                        right: colRight.left
                        rightMargin: units.gu(1)
                        topMargin: units.gu(1)
                    }

                    Label {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        text: modelData.title
                        color: modelData.updateState ? "gray" : "black"
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Label {
                        id: labelVersion
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        text: modelData.remoteVersion ? i18n.tr("Version: ") + modelData.remoteVersion : ""
                        color: "black"
                        visible: !modelData.selected
                        elide: Text.ElideRight
                    }
                }
            }

            ProgressBar {
                id: progress
                objectName: "progress"
                width: textArea.width - colRight.width - units.gu(2)
                height: parent.height / 3
                anchors.left: textArea.left
                anchors.leftMargin: units.gu(1)
                anchors.bottom: textArea.bottom
                anchors.bottomMargin: units.dp(5)
                opacity: modelData.selected ? 1 : 0
                value: modelData.downloadProgress
                minimumValue: 0
                maximumValue: 100

                Behavior on opacity { PropertyAnimation { duration: UbuntuAnimation.SleepyDuration } }
            }
        }
    }

    ListItem.Standard {
        id: notification
        anchors.bottom: configuration.top
    }

    ListItem.SingleValue {
        id: configuration
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
        id: updatedNotification
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
                text: updatedNotification.text
                anchors.horizontalCenter: parent.horizontalCenter
                fontSize: "large"
            }
        }
    }

    Rectangle {
        id: installingImageUpdate
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
                text: i18n.tr("Installing update...")
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
