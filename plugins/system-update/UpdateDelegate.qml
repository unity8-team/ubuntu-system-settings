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
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Components.Themes 1.3
import Ubuntu.SystemSettings.Update 1.0

Column {
    id: update

    spacing: units.gu(2)

    property int updateState // This is an SystemUpdate::UpdateState
    property int kind // This is an SystemUpdate::UpdateKind
    property real size
    property string version
    property string downloadId
    property date updatedAt

    property alias error: errorElementDetail.text
    property alias name: nameLabel.text
    property alias iconUrl: icon.source
    property alias changelog: changelogLabel.text
    property alias progress: progressBar.value

    height: childrenRect.height

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
    signal launch()

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

    Behavior on height {
        animation: UbuntuNumberAnimation {}
    }

    Item {
        height: 1; width: 1; // Spacer
    }

    // Layouts the icon first, then the rest to the right/left of that.
    RowLayout {
        id: rootLayout
        anchors {
            left: parent.left
            leftMargin: units.gu(2)
            right: parent.right
            rightMargin: units.gu(2)
        }
        spacing: units.gu(2)

        Item {
            Layout.preferredWidth: units.gu(4)
            Layout.preferredHeight: units.gu(4)
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

            Item {
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                width: units.gu(4)
                height: width

                Image {
                    id: icon
                    visible: kind === Update.KindImage
                    anchors.fill: parent
                    asynchronous: true
                    smooth: true
                    mipmap: true
                }

                UbuntuShape {
                    visible: kind !== Update.KindImage
                    anchors.fill: parent
                    source: icon
                }
            }

        }

        // Positions everything but the icon (and divider).
        ColumnLayout {
            Layout.minimumHeight: units.gu(6)
            spacing: units.gu(1)

            // Positions name and button.
            RowLayout {
                spacing: units.gu(2)

                Label {
                    id: nameLabel
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideMiddle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Button {
                    id: button
                    objectName: "updateButton"

                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                    color: theme.palette.normal.background

                    strokeColor: "transparent"
                    visible: {
                        switch (updateState) {
                        case Update.StateInstalled:
                            // If installed, we can launch a click.
                            return kind === Update.KindClick;
                        default:
                            return true;
                        }
                    }
                    enabled: {
                        switch(updateState) {
                        case Update.StateAvailable:
                        case Update.StateDownloading:
                        case Update.StateDownloadingAutomatically:
                        case Update.StateDownloadPaused:
                        case Update.StateAutomaticDownloadPaused:
                        case Update.StateInstallPaused:
                        case Update.StateDownloaded:
                        case Update.StateFailed:
                            return true;

                        // Enabled if installed and a click app (can launch).
                        case Update.StateInstalled:
                            return kind === Update.KindClick;

                        case Update.StateInstalling:
                        case Update.StateInstallingAutomatically:
                        case Update.StateUnavailable:
                        case Update.StateInstallFinished:
                        case Update.StateQueuedForDownload:
                        case Update.StateUnknown:
                        default:
                            return false;
                        }
                    }
                    text: {
                        switch(updateState) {
                        case Update.StateUnknown:
                        case Update.StateUnavailable:
                        case Update.StateFailed:
                            return i18n.tr("Retry");

                        case Update.StateAvailable:
                        case Update.StateQueuedForDownload:
                            if (update.kind === Update.KindClick) {
                                return i18n.tr("Update");
                            } else {
                                return i18n.tr("Download");
                            }

                        case Update.StateDownloadPaused:
                        case Update.StateAutomaticDownloadPaused:
                        case Update.StateInstallPaused:
                                return i18n.tr("Resume");

                        case Update.StateDownloading:
                        case Update.StateDownloadingAutomatically:
                        case Update.StateInstalling:
                        case Update.StateInstallingAutomatically:
                        case Update.StateInstallFinished:
                            return i18n.tr("Pause");

                        case Update.StateDownloaded:
                            if (kind === Update.KindImage) {
                                return i18n.tr("Install…");
                            } else {
                                return i18n.tr("Install");
                            }

                        case Update.StateInstalled:
                            return i18n.tr("Open");

                        default:
                            console.error("Unknown update state", updateState);
                        }
                    }

                    onClicked: {
                        switch (updateState) {

                        // Retries.
                        case Update.StateUnknown:
                        case Update.StateUnavailable:
                        case Update.StateFailed:
                            update.retry();
                            break;

                        case Update.StateDownloadPaused:
                        case Update.StateAutomaticDownloadPaused:
                        case Update.StateInstallPaused:
                            update.resume();
                            break;

                        case Update.StateAvailable:
                            if (kind === Update.KindClick) {
                                update.install();
                            } else {
                                update.download();
                            }
                            break;

                        case Update.StateDownloaded:
                                update.install();
                                break;

                        case Update.StateDownloading:
                        case Update.StateDownloadingAutomatically:
                        case Update.StateInstalling:
                        case Update.StateInstallingAutomatically:
                            update.pause();
                            break;

                        case Update.StateInstalled:
                            update.launch();
                            break;
                        }
                    }
                } // Button
            } // Name/button RowLayout

            RowLayout {
                spacing: units.gu(2)

                ChangelogExpander {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                    enabled: update.changelog !== ""
                    version: update.version
                    visible: updateState !== Update.StateFailed && downloadLabel.text === ""
                    expanded: changelogCol.visible
                    onClicked: changelogCol.visible = !changelogCol.visible
                }

                Label {
                    id: downloadLabel
                    objectName: "updateDownloadLabel"

                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                    visible: text !== ""

                    fontSize: "small"
                    text: {
                        switch (updateState) {

                        case Update.StateInstalling:
                        case Update.StateInstallingAutomatically:
                        case Update.StateInstallPaused:
                            return i18n.tr("Installing");

                        case Update.StateInstallPaused:
                        case Update.StateDownloadPaused:
                            return i18n.tr("Paused");

                        case Update.StateQueuedForDownload:
                            return i18n.tr("Waiting to download");

                        case Update.StateDownloading:
                            return i18n.tr("Downloading");

                        // case Update.StateFailed:
                        //     return i18n.tr("Installation failed");

                        default:
                            return "";
                        }
                    }
                }

                Label {
                    id: statusLabel
                    objectName: "updateStatusLabel"

                    visible: text !== ""
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.fillWidth: true

                    horizontalAlignment: Text.AlignRight
                    fontSize: "small"

                    text: {
                        switch (updateState) {
                        case Update.StateAvailable:
                            return formatter(size);

                        case Update.StateDownloading:
                        case Update.StateDownloadingAutomatically:
                        case Update.StateDownloadPaused:
                        case Update.StateAutomaticDownloadPaused:
                            var down = formatter((size / 100) * progress);
                            var left = formatter(size);
                            /* TRANSLATORS: %1 is the human readable amount
                            of bytes downloaded, and %2 is the total to be
                            downloaded. */
                            return i18n.tr("%1 of %2").arg(down).arg(left);

                        case Update.StateDownloaded:
                            return i18n.tr("Downloaded");

                        case Update.StateInstallFinished:
                            return i18n.tr("Installed");

                        case Update.StateInstalled:
                            /* TRANSLATORS: %1 is the date at which this
                            update was applied. */
                            return i18n.tr("Updated at %1").arg(
                                updatedAt.toLocaleDateString(Qt.locale(), "d MMMM")
                            );

                        default:
                            return "";
                        }
                    }

                    /* Seems AlignRight causes a rendering issue that can be
                    fixed by doing an explicit doLayout. */
                    Component.onCompleted: doLayout()
                }
            }

            Column {
                id: error
                objectName: "updateError"
                spacing: units.gu(1)
                height: childrenRect.height
                Layout.fillWidth: true
                visible: errorElementDetail.text

                Label {
                    id: errorElementTitle
                    text: i18n.tr("Update failed")
                    color: UbuntuColors.red
                }

                Label {
                    id: errorElementDetail
                    anchors { left: parent.left; right: parent.right }
                    fontSize: "small"
                    wrapMode: Text.WrapAnywhere
                }
            } // Error column

            ProgressBar {
                id: progressBar
                objectName: "updateProgressbar"

                visible: {
                    switch (updateState) {
                    case Update.StateQueuedForDownload:
                    case Update.StateDownloading:
                    case Update.StateDownloadPaused:
                    case Update.StateInstalling:
                    case Update.StateInstallPaused:
                        return true;

                    default:
                        return false;
                    }
                }
                Layout.maximumHeight: units.gu(0.5)
                Layout.fillWidth: true
                // Layout.topMargin: units.gu(1)

                indeterminate: update.progress < 0 || update.progress > 100
                minimumValue: 0
                maximumValue: 100
                showProgressPercentage: false
            } // Progress bar

            ChangelogExpander {
                Layout.fillWidth: true
                // Layout.topMargin: units.gu(2)

                version: update.version
                enabled: update.changelog !== ""
                visible: updateState !== Update.StateFailed && downloadLabel.text !== ""
                expanded: changelogCol.visible
                onClicked: changelogCol.visible = !changelogCol.visible
            }

            Column {
                id: changelogCol

                visible: false
                height: childrenRect.height

                Layout.fillWidth: true
                // Layout.topMargin: units.gu(2)

                opacity: visible ? 1 : 0

                Behavior on opacity {
                    animation: UbuntuNumberAnimation {}
                }

                Label {
                    id: changelogLabel
                    anchors { left: parent.left; right: parent.right }
                    fontSize: "small"
                    wrapMode: Text.WordWrap
                }
            }
        } // Layout for the rest of the stuff
    } // Icon and rest layout

    ListItems.ThinDivider {}

    Timer {
        id: hideTimer
        interval: 2000
        running: update.updateState === Update.StateInstallFinished
        onTriggered: update.state = "finished"
    }
}
