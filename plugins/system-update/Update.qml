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
 * Visually represents an update, and accepts user input. It does NOT
 * know how to perform downloads and updates:
 * This component is meant to be software agnostic, meaning it doesn't
 * care whether or not it is a Click/Snappy/System update.
 */

import QtQuick 2.4
import QtQuick.Layouts 1.2
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Components.Themes 1.3
import Ubuntu.SystemSettings.Update 1.0

Item {
    id: update

    property int status // This is an UpdateManager::UpdateStatus
    property int mode // This is an UpdateManager::UpdateMode
    property int size
    property string version
    property string errorTitle
    property string errorDetail

    property alias name: name.text
    property alias iconUrl: icon.source
    property alias changelog: changelogLabel.text
    property alias progress: progressBar.value

    property var formatter: function (number) { return number }

    signal retry()
    signal download()
    signal pause()
    signal install()

    function setError (title, detail) {
        errorTitle = title;
        errorDetail = detail;
    }

    height: (topSlot.height + topSlot.anchors.topMargin
             + middleSlot.height + middleSlot.anchors.topMargin
             + progressBarSlot.height + progressBarSlot.anchors.topMargin
             + bottomSlot.height + bottomSlot.anchors.topMargin
             + divider.height + divider.anchors.topMargin)

    Item {
        id: iconSlot
        anchors {
            top: update.top
            left: update.left
            bottom: update.bottom
        }
        width: units.gu(9)

        Icon {
            id: icon
            anchors {
                left: iconSlot.left
                right: iconSlot.right
                top: iconSlot.top
                margins: units.gu(2)
            }
        }
    }

    RowLayout {
        id: topSlot
        height: units.gu(4)
        spacing: units.gu(1)
        anchors {
            left: iconSlot.right
            top: update.top
            topMargin: units.gu(2)
            right: update.right
            rightMargin: units.gu(2)
        }

        Label {
            id: name
            verticalAlignment: Text.AlignVCenter
            height: button.height
            font.pointSize: 10
            elide: Text.ElideMiddle
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
        }

        Button {
            id: button
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            Layout.fillHeight: true

            // Enabled as long as it's not NonPausable.
            enabled: update.mode !== UpdateManager.NonPausable

            text: {
                switch(update.mode) {
                case UpdateManager.Retriable:
                    return i18n.tr("Retry")
                case UpdateManager.Downloadable:
                    return i18n.tr("Download")
                case UpdateManager.Pausable:
                case UpdateManager.NonPausable:
                    return i18n.tr("Pause")
                case UpdateManager.Installable:
                    return i18n.tr("Install")
                case UpdateManager.InstallableWithRestart:
                    return i18n.tr("Install…")
                default:
                    console.warn("Unknown update mode", update.mode);
                }
            }

            onClicked: {
                switch (update.mode) {
                case UpdateManager.Retriable:
                    update.retry();
                    break;
                case UpdateManager.Downloadable:
                    update.download();
                    break;
                case UpdateManager.Pausable:
                    update.pause();
                    break;
                case UpdateManager.Installable:
                case UpdateManager.InstallableWithRestart:
                    update.install();
                    break;
                case UpdateManager.NonPausable:
                    break;
                }
            }
        }
    }

    Column {
        id: errorSlot
        spacing: units.gu(1)
        anchors {
            top: topSlot.bottom
            left: iconSlot.right
            right: update.right
            rightMargin: units.gu(2)

        }
        visible: update.status === UpdateManager.Failed
        Label {
            text: update.errorTitle
            color: UbuntuColors.red
        }

        Label {
            anchors { left: parent.left; right: parent.right }
            text: update.errorDetail
            fontSize: "small"
            wrapMode: Text.WrapAnywhere
        }
    }

    RowLayout {
        id: progressBarSlot
        anchors {
            top: topSlot.bottom
            topMargin: units.gu(1)
            left: iconSlot.right
            right: update.right
            rightMargin: units.gu(2)
        }
        visible: {
            switch (update.status) {
            case UpdateManager.AutomaticallyDownloading:
            case UpdateManager.ManuallyDownloading:
            case UpdateManager.DownloadPaused:
            case UpdateManager.InstallationPaused:
            case UpdateManager.Installing:
                return true;
            case UpdateManager.Failed:
            default:
                return false;
            }
        }
        height: units.gu(2)

        ProgressBar {
            id: progressBar
            anchors { left: parent.left }
            indeterminate: progress < 0 || progress > 100
            minimumValue: 0
            maximumValue: 100
            showProgressPercentage: false
            Layout.maximumWidth: units.gu(23)
            Layout.maximumHeight: units.gu(0.5)
        }

        Rectangle {
            id: progressBarInfo
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
        }
    }

    RowLayout {
        id: middleSlot
        height: units.gu(3)

        anchors {
            top: topSlot.bottom
            topMargin: progressBarSlot.visible ? units.gu(1) : 0
            left: iconSlot.right
            right: update.right
            rightMargin: units.gu(2)
        }

        states: [
            State {
                when: progressBarSlot.visible
                AnchorChanges { target: middleSlot; anchors.top: progressBarSlot.bottom }
            },
            State {
                when: update.status === UpdateManager.Failed
                AnchorChanges { target: middleSlot; anchors.top: errorSlot.bottom }
            }
        ]

        // IMPROVE?
        // transitions: Transition {
        //     AnchorAnimation {
        //         duration: UbuntuAnimation.FastDuration
        //         easing: UbuntuAnimation.StandardEasing
        //     }
        // }

        RowLayout {
            id: expandableVersionLabel
            property bool expanded: false
            spacing: units.gu(0.5)
            Label {
                id: versionLabel
                verticalAlignment: Text.AlignVCenter
                text: i18n.tr("Version %1").arg(update.version)
                fontSize: "small"
            }

            Icon {
                id: expandableVersionIcon
                name: "info"
                visible: update.changelog
                color: parent.expanded ? theme.palette.selected.baseText : theme.palette.normal.baseText

                height: versionLabel.height
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (!update.changelog) return;
                    parent.expanded = !parent.expanded
                }
            }
        }

        Label {
            id: sizeAndStatusLabel
            states: State {
                when: progressBarSlot.visible
                ParentChange { target: sizeAndStatusLabel;
                               parent: progressBarInfo }
                PropertyChanges { target: sizeAndStatusLabel;
                                  width: progressBarInfo.width;
                                  horizontalAlignment: Text.AlignRight }
            }
            verticalAlignment: Text.AlignVCenter
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            fontSize: "small"
            text: {
                switch (update.status) {
                case UpdateManager.NotStarted:
                    return formatter(size);
                case UpdateManager.AutomaticallyDownloading:
                case UpdateManager.ManuallyDownloading:
                case UpdateManager.DownloadPaused:
                    var down = formatter(size * (progressBar.value / 100));
                    var left = formatter(size);
                    return i18n.tr("%1 of %2").arg(down).arg(left);
                case UpdateManager.NotAvailable:
                case UpdateManager.Failed:
                case UpdateManager.Installing:
                case UpdateManager.Installed:
                case UpdateManager.InstallationPaused:
                default:
                    return "";
                }
            }
        }
    }

    Item {
        id: bottomSlot
        anchors {
            top: middleSlot.bottom
            left: iconSlot.right
            right: update.right
            rightMargin: units.gu(2)
        }
        height: childrenRect.height

        Column {
            id: changelogCol
            // Whether or not to animate height. We will enable this
            // when changelogLabel has completed.
            property bool animate: false
            height: childrenRect.height
            Behavior on height {
                animation: UbuntuNumberAnimation {}
                enabled: changelogCol.animate
            }
            anchors { left: parent.left; right: parent.right }

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
    }

    ListItems.ThinDivider {
        id: divider
        anchors {
            top: bottomSlot.bottom
            topMargin: units.gu(1)
            left: iconSlot.right
            right: update.right
            rightMargin: units.gu(2)
        }
    }
}
