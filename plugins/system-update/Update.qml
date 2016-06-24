/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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
 * Visually represents an update, and accepts user input. It does NOT
 * know how to perform downloads and updates:
 * This component is meant to be software agnostic, meaning it doesn't
 * care whether or not it is a Click/Snappy/System update.
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Components.Themes 1.3
import Ubuntu.SystemSettings.Update 1.0

Item {
    id: update

    property int updateState // This is an SystemUpdate::UpdateState
    property int kind // This is an SystemUpdate::UpdateKind
    property int size
    property string version
    property string errorTitle
    property string errorDetail

    property alias name: nameLabel.text
    property alias iconUrl: icon.source
    property alias changelog: changelogLabel.text
    property alias progress: progressBar.value

    // By Aliceljm [1].
    // [1] http://stackoverflow.com/a/18650828/538866
    property var formatter: function formatBytes(bytes, decimals) {
        if (typeof Utilities !== "undefined") {
            return Utilities.formatSize(bytes);
        }
        if (typeof decimals === 'undefined') decimals = 0;
        if(bytes == 0) return '0 Byte';
        var k = 1000; // or 1024 for binary
        var dm = decimals + 1 || 3;
        var sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
        var i = Math.floor(Math.log(bytes) / Math.log(k));
        return parseFloat((bytes / Math.pow(k, i)).toFixed(dm)) + " " + sizes[i];
    }

    signal retry()
    signal download()
    signal pause()
    signal resume()
    signal install()

    // states: State {
    //     name: "done"; when: status === SystemUpdate.Installed
    //     PropertyChanges { target: update; height: 0; clip: true }
    // }

    // transitions: Transition {
    //     from: ""; to: "done"
    //     SequentialAnimation {
    //         // Dummy, since PauseAnimation is buggy here.
    //         PropertyAnimation { property: "opacity"; duration: 2000 }
    //         PropertyAnimation {
    //             target: update
    //             properties: "height"
    //             duration: UbuntuAnimation.FastDuration
    //             easing: UbuntuAnimation.StandardEasing
    //         }
    //     }
    // }

    function setError (title, detail) {
        errorTitle = title;
        errorDetail = detail;
    }

    states: [
        State {
            name: "finished"
            PropertyChanges {
                target: update
                height: 0
                clip: true
            }
        }
    ]

    height: {
        var h = 0;
        var nameHeight = nameLabel.height + nameLabel.anchors.topMargin;
        var buttonHeight = button.height + button.anchors.topMargin;
        var versionHeight = expandableVersionLabel.height + expandableVersionLabel.anchors.topMargin;

        var statusLabelHeight = statusLabel.text ?
                (statusLabel.height + statusLabel.anchors.topMargin) : 0;
        var downloadLabelHeight = downloadLabel.text ?
            (downloadLabel.height + downloadLabel.anchors.topMargin) : 0;

        var progressBarHeight = progressBar.visible ?
                (progressBar.height + progressBar.anchors.topMargin) : 0;
        var changelogLabelHeight = changelogLabel.visible ?
                (changelogCol.height + changelogCol.anchors.topMargin) : 0;
        var errorHeight = error.visible ? (error.height + units.gu(1)) : 0;

        h += Math.max(nameHeight, buttonHeight);
        if (progressBarHeight || errorHeight) {
            h += progressBarHeight
                 + errorHeight
                 + versionHeight
                 + Math.max(downloadLabelHeight, statusLabelHeight);
        } else {
            h += Math.max(downloadLabelHeight, statusLabelHeight, versionHeight);
        }

        h += changelogLabelHeight;
        return h;
    }

    Behavior on height {
        animation: UbuntuNumberAnimation {}
    }


    // Holds icon, but also extends vertically to the bottom of the component.
    Item {
        id: iconSlot
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }
        width: units.gu(9)

        Icon {
            id: icon
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: units.gu(2)
            }
        }
    }

    Label {
        id: nameLabel
        anchors {
            top: parent.top
            topMargin: units.gu(2)
            left: iconSlot.right
            right: button.left
        }
        verticalAlignment: Text.AlignVCenter
        height: button.height
        elide: Text.ElideMiddle
    }

    Button {
        id: button
        objectName: "updateButton"
        anchors {
            top: parent.top
            topMargin: units.gu(2.3) // Vcenter in name does not work here
            right: parent.right
            rightMargin: units.gu(2)
        }

        visible: {
            switch (updateState) {
            case SystemUpdate.StateInstalled:
                return false;
            default:
                return true;
            }
        }
        enabled: {
            switch(updateState) {
            case SystemUpdate.StateAvailable:
            case SystemUpdate.StateQueuedForDownload:
            case SystemUpdate.StateDownloading:
            case SystemUpdate.StateDownloadingAutomatically:
            case SystemUpdate.StateDownloadPaused:
            case SystemUpdate.StateAutomaticDownloadPaused:
            case SystemUpdate.StateInstallPaused:
            case SystemUpdate.StateDownloaded:
            case SystemUpdate.StateFailed:
                return true;

            case SystemUpdate.StateInstalling:
            case SystemUpdate.StateInstallingAutomatically:
            case SystemUpdate.StateUnavailable:
            case SystemUpdate.StateInstalled:
            case SystemUpdate.StateInstallFinished:
            case SystemUpdate.StateUnknown:
            default:
                return false;
            }
        }
        text: {
            switch(updateState) {
            case SystemUpdate.StateUnknown:
            case SystemUpdate.StateUnavailable:
            case SystemUpdate.StateFailed:
                return i18n.tr("Retry");

            case SystemUpdate.StateAvailable:
                if (kind === SystemUpdate.KindApp) {
                    return i18n.tr("Update");
                } else {
                    return i18n.tr("Download");
                }

            case SystemUpdate.StateDownloadPaused:
            case SystemUpdate.StateAutomaticDownloadPaused:
                    return i18n.tr("Resume");

            case SystemUpdate.StateQueuedForDownload:
            case SystemUpdate.StateDownloading:
            case SystemUpdate.StateDownloadingAutomatically:
            case SystemUpdate.StateInstalling:
            case SystemUpdate.StateInstallingAutomatically:
            case SystemUpdate.StateInstallFinished:
            case SystemUpdate.StateInstalled:
                return i18n.tr("Pause");

            case SystemUpdate.StateInstallPaused:
            case SystemUpdate.StateDownloaded:
                if (kind === SystemUpdate.KindSystem) {
                    return i18n.tr("Install…");
                } else {
                    return i18n.tr("Install");
                }

            default:
                console.error("Unknown update state", updateState);
            }
        }

        onClicked: {
            switch (updateState) {

            // Retries.
            case SystemUpdate.StateUnknown:
            case SystemUpdate.StateUnavailable:
            case SystemUpdate.StateFailed:
                update.retry();
                break;

            case SystemUpdate.StateDownloadPaused:
            case SystemUpdate.StateAutomaticDownloadPaused:
            case SystemUpdate.StateInstallPaused:
                update.resume();
                break;

            case SystemUpdate.StateAvailable:
                if (kind === SystemUpdate.KindApp) {
                    update.install();
                } else {
                    update.download();
                }
                break;

            case SystemUpdate.StateDownloaded:
                    update.install();
                    break;

            case SystemUpdate.StateQueuedForDownload:
            case SystemUpdate.StateDownloading:
            case SystemUpdate.StateDownloadingAutomatically:
            case SystemUpdate.StateInstalling:
            case SystemUpdate.StateInstallingAutomatically:
                update.pause();
                break;
            }
        }
    }

    RowLayout {
        id: expandableVersionLabel
        objectName: "updateVersionLabel"
        anchors {
            top: nameLabel.bottom
            topMargin: units.gu(1)
            left: iconSlot.right
        }
        width: units.gu(10)
        property bool expanded: false
        spacing: units.gu(0.5)
        visible: updateState !== SystemUpdate.StateFailed

        Label {
            id: versionLabel
            verticalAlignment: Text.AlignVCenter
            text: i18n.tr("Version %1").arg(update.version)
            fontSize: "small"
            elide: Text.ElideMiddle
        }

        Icon {
            id: expandableVersionIcon
            name: "info"
            visible: update.changelog
            color: parent.expanded ? theme.palette.selected.activity : theme.palette.normal.baseText

            height: versionLabel.height
        }

        MouseArea {
            anchors {
                fill: parent
                margins: units.gu(-2) // grow hitbox
            }
            onClicked: {
                if (!update.changelog) return;
                parent.expanded = !parent.expanded
            }
        }
    }

    Label {
        id: downloadLabel
        objectName: "updateDownloadLabel"
        anchors {
            top: nameLabel.bottom
            topMargin: units.gu(1)
            left: iconSlot.right
        }
        width: units.gu(10)
        verticalAlignment: Text.AlignVCenter
        fontSize: "small"
        text: {
            switch (updateState) {

            case SystemUpdate.StateInstalling:
            case SystemUpdate.StateInstallingAutomatically:
            case SystemUpdate.StateInstallPaused:
                return i18n.tr("Installing");

            case SystemUpdate.StateInstallPaused:
            case SystemUpdate.StateDownloadPaused:
                return i18n.tr("Paused");

            case SystemUpdate.StateQueuedForDownload:
                return i18n.tr("Waiting to download");

            case SystemUpdate.StateDownloading:
                return i18n.tr("Downloading");

            // case SystemUpdate.StateFailed:
            //     return i18n.tr("Installation failed");

            default:
                return "";
            }
        }
    }


    Label {
        id: statusLabel
        objectName: "updateStatusLabel"
        anchors {
            top: nameLabel.bottom
            topMargin: units.gu(1)
            right: parent.right
            rightMargin: units.gu(2)
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        fontSize: "small"
        text: {
            switch (updateState) {

            case SystemUpdate.StateAvailable:
                return formatter(size);

            case SystemUpdate.StateDownloading:
            case SystemUpdate.StateDownloadingAutomatically:
            case SystemUpdate.StateDownloadPaused:
            case SystemUpdate.StateAutomaticDownloadPaused:
                var down = formatter(size * progress);
                var left = formatter(size);
                // TRANSLATORS: %1 is the human readable amount of bytes
                // downloaded, and %2 is the total to be downloaded.
                return i18n.tr("%1 of %2").arg(down).arg(left);

            case SystemUpdate.StateDownloaded:
                return i18n.tr("Downloaded");

            case SystemUpdate.StateInstallFinished:
                return i18n.tr("Installed");

            default:
                return "";
            }
        }
    }

    Column {
        id: error
        objectName: "updateError"
        spacing: units.gu(1)
        height: childrenRect.height

        property alias title: errorElementTitle.text
        property alias detail: errorElementDetail.text

        anchors {
            top: nameLabel.bottom
            left: iconSlot.right
            right: parent.right
            rightMargin: units.gu(2)

        }
        states: State {
            name: "visible"
            when: updateState === SystemUpdate.StateFailed

            PropertyChanges {
                target: error
                visible: true
            }

            AnchorChanges {
                target: expandableVersionLabel
                anchors.top: error.bottom
            }
        }

        visible: false

        Label {
            id: errorElementTitle
            text: update.errorTitle
            color: UbuntuColors.red
        }

        Label {
            id: errorElementDetail
            anchors { left: parent.left; right: parent.right }
            text: update.errorDetail
            fontSize: "small"
            wrapMode: Text.WrapAnywhere
        }
    }


    ProgressBar {
        id: progressBar
        objectName: "updateProgressbar"
        anchors {
            top: downloadLabel.bottom
            topMargin: units.gu(1)
            right: parent.right
            rightMargin: units.gu(2)
            left: iconSlot.right
        }

        states: State {
            name: "visible"
            when: {
                // Waiting to download manually, downloading manually,
                // and/or installing manually
                switch (updateState) {

                case SystemUpdate.StateQueuedForDownload:
                case SystemUpdate.StateDownloading:
                case SystemUpdate.StateDownloadPaused:
                case SystemUpdate.StateInstalling:
                case SystemUpdate.StateInstallPaused:
                    return true;

                default:
                    return false;
                }
            }

            PropertyChanges {
                target: progressBar
                visible: true
            }

            AnchorChanges {
                target: expandableVersionLabel
                anchors.top: progressBar.bottom
            }
        }
        visible: false
        height: units.gu(0.5)
        indeterminate: update.progress < 0 || update.progress > 100
        minimumValue: 0
        maximumValue: 100
        showProgressPercentage: false
    }

    // RowLayout {
    //     id: progressBarSlot
    //     anchors {
    //         top: topSlot.bottom
    //         topMargin: units.gu(1)
    //         left: iconSlot.right
    //         right: update.right
    //         rightMargin: units.gu(2)
    //     }
    //     visible: {
    //         switch (updateState) {
    //         case SystemUpdate.AutomaticallyDownloading:
    //         case SystemUpdate.ManuallyDownloading:
    //         case SystemUpdate.DownloadPaused:
    //         case SystemUpdate.InstallationPaused:
    //         case SystemUpdate.Installing:
    //             return true;
    //         case SystemUpdate.Failed:
    //         default:
    //             return false;
    //         }
    //     }
    //     height: units.gu(2)

    //     ProgressBar {
    //         id: progressBar
    //         anchors { left: parent.left }
    //         indeterminate: progress < 0 || progress > 100
    //         minimumValue: 0
    //         maximumValue: 100
    //         showProgressPercentage: false
    //         Layout.fillWidth: true
    //         Layout.maximumHeight: units.gu(0.5)
    //     }

    //     Rectangle {
    //         id: progressBarInfo
    //         Layout.fillHeight: true
    //         Layout.minimumWidth: units.gu(12)
    //         Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
    //     }
    // }

    // // RowLayout {
    // //     id: middleSlot
    // //     height: units.gu(3)

    // //     anchors {
    // //         top: topSlot.bottom
    // //         topMargin: progressBarSlot.visible ? units.gu(1) : 0
    // //         left: iconSlot.right
    // //         right: update.right
    // //         rightMargin: units.gu(2)
    // //     }

    // //     states: [
    // //         State {
    // //             when: progressBarSlot.visible
    // //             AnchorChanges { target: middleSlot; anchors.top: progressBarSlot.bottom }
    // //         },
    // //         State {
    // //             when: updateState === SystemUpdate.Failed
    // //             AnchorChanges { target: middleSlot; anchors.top: errorSlot.bottom }
    // //         }
    // //     ]

    // //     // IMPROVE?
    // //     // transitions: Transition {
    // //     //     AnchorAnimation {
    // //     //         duration: UbuntuAnimation.FastDuration
    // //     //         easing: UbuntuAnimation.StandardEasing
    // //     //     }
    // //     // }

    // // }

    // Item {
    //     id: bottomSlot
    //     anchors {
    //         top: middleSlot.bottom
    //         left: iconSlot.right
    //         right: update.right
    //         rightMargin: units.gu(2)
    //     }
    //     height: childrenRect.height

    Column {
        id: changelogCol
        anchors {
            top: expandableVersionLabel.bottom
            topMargin: units.gu(1)
            left: iconSlot.right
            right: parent.right
        }

        // Whether or not to animate height. We will enable this
        // when changelogLabel has completed.
        property bool animate: false
        height: childrenRect.height

        Behavior on height {
            animation: UbuntuNumberAnimation {}
            enabled: changelogCol.animate
        }

        add: Transition {
            SequentialAnimation {
                // Wait out height animation
                NumberAnimation {
                    property: "opacity"
                    from: 0; to: 0;
                    duration: UbuntuAnimation.FastDuration
                }
                NumberAnimation {
                    property: "opacity"
                    from: 0; to: 1.0;
                    duration: UbuntuAnimation.BriskDuration
                }
            }
        }

        Label {
            id: changelogLabel
            anchors { left: parent.left; right: parent.right }
            clip: true
            fontSize: "small"
            wrapMode: Text.WordWrap
            visible: expandableVersionLabel.expanded
            property int origHeight;
            Component.onCompleted: {
                origHeight = height;
                visible = Qt.binding(function (){
                    return expandableVersionLabel.expanded;
                });
                height = Qt.binding(function () {
                    return visible && text !== "" ? origHeight : 0;
                });
                changelogCol.animate = true;
            }
        }

    }

    MouseArea {
        anchors.fill: changelogCol
        onClicked: expandableVersionLabel.expanded = false
    }

    ListItems.ThinDivider {
        id: divider
        visible: update.height !== 0
        anchors {
            top: parent.bottom
            topMargin: units.gu(1)
            left: iconSlot.right
            right: parent.right
            rightMargin: units.gu(2)
        }
    }

    Timer {
        id: hideTimer
        interval: 2000
        running: update.updateState === SystemUpdate.StateInstallFinished
        onTriggered: update.state = "finished"
    }
}
