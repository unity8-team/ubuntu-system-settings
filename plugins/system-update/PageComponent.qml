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
import QtQuick 2.0
import QtSystemInfo 5.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import Ubuntu.OnlineAccounts.Client 0.1
import Ubuntu.SystemSettings.Update 1.0


ItemPage {
    id: root
    objectName: "systemUpdatesPage"

    title: installingImageUpdate.visible ? "" : i18n.tr("Updates")
    flickable: installingImageUpdate.visible ? null : scrollWidget

    property bool installAll: false
    property bool includeSystemUpdate: false
    property bool systemUpdateInProgress: false
    property int updatesAvailable: 0
    property bool isCharging: false
    property bool batterySafeForUpdate: isCharging || chargeLevel > 25
    property var chargeLevel: indicatorPower.batteryLevel || 0
    property var notificationAction;

    QDBusActionGroup {
        id: indicatorPower
        busType: 1
        busName: "com.canonical.indicator.power"
        objectPath: "/com/canonical/indicator/power"
        property variant batteryLevel: action("battery-level").state
        Component.onCompleted: start()
    }

    DeviceInfo {
        id: deviceInfo
    }

    BatteryInfo {
        id: batteryInfo
        monitorChargingState: true

        onChargingStateChanged: {
            if (state === BatteryInfo.Charging) {
                isCharging = true
            }
            else if (state === BatteryInfo.Discharging &&
                     batteryInfo.batteryStatus(0) !== BatteryInfo.BatteryFull) {
                isCharging = false
            }
            else if (batteryInfo.batteryStatus(0) === BatteryInfo.BatteryFull ||
                     state === BatteryInfo.NotCharging) {
                isCharging = true
            }
        }
        Component.onCompleted: {
            onChargingStateChanged(0, chargingState(0))
        }
    }

    Setup {
        id: uoaConfig
        applicationId: "ubuntu-system-settings"
        providerId: "ubuntuone"
    }

    Component {
         id: dialogInstallComponent
         Dialog {
             id: dialogueInstall
             title: i18n.tr("Update System")
             text: root.batterySafeForUpdate ? i18n.tr("The phone needs to restart to install the system update.") : i18n.tr("Connect the phone to power before installing the system update.")

             Button {
                 text: i18n.tr("Install & Restart")
                 visible: root.batterySafeForUpdate ? true : false
                 color: UbuntuColors.orange
                 onClicked: {
                     installingImageUpdate.visible = true;
                     UpdateManager.applySystemUpdate();
                     PopupUtils.close(dialogueInstall);
                 }
             }
             Button {
                 text: i18n.tr("Not Now")
                 color: UbuntuColors.warmGrey
                 onClicked: {
                     updateList.currentIndex = 0;
                     var item = updateList.currentItem;
                     var modelItem = UpdateManager.model[0];
                     item.actionButton.text = i18n.tr("Install");
                     item.progressBar.opacity = 0;
                     modelItem.updateReady = true;
                     modelItem.selected = false;
                     root.systemUpdateInProgress = false;
                     PopupUtils.close(dialogueInstall);
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

        onSystemUpdateDownloaded: {
            root.installAll = false;
        }

        onSystemUpdateFailed: {
            root.state = "SYSTEMUPDATEFAILED";
        }

        onUpdateProcessFailed: {
            root.state = "SYSTEMUPDATEFAILED";
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
            
            ListItem.Base {
                id: checkForUpdatesArea
                objectName: "checkForUpdatesArea"
                showDivider: false
                visible: false

                ActivityIndicator {
                    id: activity
                    running: checkForUpdatesArea.visible
                    visible: activity.running
                    anchors {
                        left: parent.left
                        top: parent.top
                        rightMargin: units.gu(2)
                    }
                    height: parent.height
                }

                Label {
                    text: i18n.tr("Checking for updates…")
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    anchors {
                        left: activity.right
                        top: parent.top
                        leftMargin: units.gu(2)
                    }
                    height: parent.height
                }
            }

            ListItem.SingleControl {
                height: installAllButton.visible ? units.gu(8) : units.gu(2)
                highlightWhenPressed: false
                control: Button {
                    id: installAllButton
                    objectName: "installAllButton"
                    property string primaryText: includeSystemUpdate ?
                                                     i18n.tr("Install %1 update…", "Install %1 updates…", root.updatesAvailable).arg(root.updatesAvailable) :
                                                     i18n.tr("Install %1 update", "Install %1 updates", root.updatesAvailable).arg(root.updatesAvailable)
                    property string secondaryText: i18n.tr("Pause All")
                    color: UbuntuColors.orange
                    text: root.installAll ? secondaryText : primaryText
                    width: parent.width - units.gu(4)

                    onClicked: {
                        root.installAll = !root.installAll;
                        for (var i=0; i < updateList.count; i++) {
                            updateList.currentIndex = i;
                            var item = updateList.currentItem;
                            var modelItem = UpdateManager.model[i];
                            if (modelItem.updateState && !modelItem.updateReady && modelItem.selected) {
                                item.pause();
                                continue;
                            }
                            if (!modelItem.updateState && !modelItem.updateReady && modelItem.selected) {
                                item.resume();
                                continue;
                            }
                            if (!modelItem.updateState && !modelItem.updateReady && !modelItem.selected) {
                                item.start();
                                continue;
                            }
                        }
                    }
                }
                showDivider: false
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
                spacing: units.gu(2)

                delegate: ListItem.Subtitled {
                    id: listItem
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    iconSource: Qt.resolvedUrl(modelData.iconUrl)
                    iconFrame: modelData.systemUpdate ? false : true
                    height: textArea.height
                    highlightWhenPressed: false
                    showDivider: false

                    property alias actionButton: buttonAppUpdate
                    property alias progressBar: progress
                    property bool installing: modelData.updateReady || (progressBar.value === progressBar.maximumValue)

                    function pause () {
                        console.warn("PAUSE: " + modelData.packageName);
                        modelData.updateState = false;
                        tracker.pause();
                    }

                    function resume () {
                        console.warn("RESUME: " + modelData.packageName);
                        modelData.updateState = true;
                        tracker.resume();
                    }

                    function start () {
                        console.warn("START: " + modelData.packageName);
                        modelData.selected = true;
                        modelData.updateState = true;
                        UpdateManager.startDownload(modelData.packageName);
                    }
                    Column {
                        id: textArea
                        objectName: "textArea"
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        spacing: units.gu(0.5)
                        property string message: modelData.error
                        property bool retry: false

                        onMessageChanged: {
                            if(message.length > 0) {
                                labelVersion.text = message;
                                modelData.updateState = false;
                                modelData.selected = false;
                                textArea.retry = true;
                            }
                        }

                        Item {
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            height: buttonAppUpdate.height
                            
                            Label {
                                id: labelTitle
                                objectName: "labelTitle"
                                anchors {
                                    left: parent.left
                                    verticalCenter: parent.verticalCenter
                                }
                                text: modelData.title
                                font.bold: true
                                elide: buttonAppUpdate.visible ? Text.ElideRight : Text.ElideNone
                            }

                            Button {
                                id: buttonAppUpdate
                                objectName: "buttonAppUpdate"
                                anchors.right: parent.right
                                height: labelTitle.height + units.gu(1)
                                text: {
                                    if (textArea.retry) {
                                        return i18n.tr("Retry");
                                    }
                                    if (modelData.systemUpdate) {
                                        if (modelData.updateReady) {
                                            return i18n.tr("Install…");
                                        } else if (!modelData.updateState && !modelData.selected) {
                                            return i18n.tr("Download");
                                        }
                                    }
                                    if (modelData.updateState) {
                                        return i18n.tr("Pause");
                                    } else if (modelData.selected) {
                                        return i18n.tr("Resume");
                                    }
                                    return i18n.tr("Update");
                                }

                                function handle_update() {
                                    if (modelData.updateState && !modelData.systemUpdate)
                                        return pause();
                                    if (!modelData.updateState && !modelData.systemUpdate && modelData.selected)
                                        return resume();
                                    if (!modelData.updateState && !modelData.systemUpdate && !modelData.selected)
                                        return start();

                                    if (textArea.retry) {
                                        textArea.retry = false;
                                        UpdateManager.retryDownload(modelData.packageName);
                                        return;
                                    } else if (modelData.updateReady) {
                                        PopupUtils.open(dialogInstallComponent);
                                        return;
                                    } else if (modelData.updateState) {
                                        if (modelData.systemUpdate) {
                                            return UpdateManager.pauseDownload(modelData.packageName);
                                        }
                                    }
                                 }

                                onClicked: {
                                    handle_update();
                                }
                            }
                        } 
                        
                        Item {
                            id: labelUpdateStatus
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            height: childrenRect.height
                            visible: opacity > 0
                            opacity: modelData.selected && !modelData.updateReady ? 1 : 0
                            Behavior on opacity { PropertyAnimation { duration: UbuntuAnimation.SleepyDuration } }
                            Label {
                                objectName: "labelUpdateStatus"
                                anchors.left: parent.left
                                fontSize: "small"
                                text: installing ? i18n.tr("Installing") : i18n.tr("Downloading")
                            }
                            Label {
                                anchors.right: parent.right
                                visible: !labelSize.visible && !installing
                                fontSize: "small"
                                text: {
                                    if (!labelUpdateStatus.visible)
                                        return convert_bytes_to_size(modelData.binaryFilesize);

                                    return i18n.tr("%1 of %2").arg(
                                        convert_bytes_to_size(modelData.binaryFilesize * (progress.value * 0.01))).arg(
                                        convert_bytes_to_size(modelData.binaryFilesize)
                                    );
                                }
                            }
                        }

                        ProgressBar {
                            id: progress
                            objectName: "progress"
                            height: units.gu(2)
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            visible: opacity > 0
                            opacity: modelData.selected && !modelData.updateReady && !installing ? 1 : 0
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
                                    modelData.updateRequired = false;
                                    UpdateManager.updateClickScope();
                                }

                                onProcessing: {
                                    console.warn("onProcessing: " + modelData.packageName + " " + path);
                                    installing = true;
                                    modelData.updateState = false;
                                }

                                onStarted: {
                                    console.warn("onStarted: " + modelData.packageName + " " + success);
                                    if (success)
                                        modelData.updateState = true;
                                }

                                onPaused: {
                                    console.warn("onPaused: " + modelData.packageName + " " + success);
                                    if (success)
                                        modelData.updateState = false;
                                }

                                onResumed: {
                                    console.warn("onResumed: " + modelData.packageName + " " + success);
                                    if (success)
                                        modelData.updateState = true;
                                }

                                onCanceled: {
                                    console.warn("onCanceled: " + modelData.packageName + " " + success);
                                    if (success) {
                                        modelData.updateState = false;
                                        modelData.selected = false;
                                    }
                                }

                                onErrorFound: {
                                    modelData.updateState = false;
                                    textArea.message = error;
                                }
                            }

                            Behavior on opacity { PropertyAnimation { duration: UbuntuAnimation.SleepyDuration } }
                        }

                        Item {
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            height: childrenRect.height
                            Label {
                                id: labelVersion
                                objectName: "labelVersion"
                                anchors.left: parent.left
                                text: modelData.remoteVersion ? i18n.tr("Version: ") + modelData.remoteVersion : ""
                                elide: Text.ElideRight
                                fontSize: "small"
                            }
                            
                            Label {
                                id: labelSize
                                objectName: "labelSize"
                                anchors.right: parent.right
                                text: convert_bytes_to_size(modelData.binaryFilesize)
                                fontSize: "small"
                                visible: !labelUpdateStatus.visible && !installing
                            }
                        }
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
                ListItem.ThinDivider {}

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
                source: Qt.resolvedUrl("file:///usr/share/ubuntu/settings/system/icons/distributor-logo.png")
                anchors.horizontalCenter: parent.horizontalCenter
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
        ListItem.ThinDivider {}
        ListItem.SingleValue {
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
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("Configuration.qml"))
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
