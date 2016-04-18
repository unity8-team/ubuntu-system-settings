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

    property int status // This is an UM::UpdateStatus
    property int mode // This is an UM::UpdateMode
    property string size
    property string version

    property alias name: name.text
    property alias iconUrl: icon.source
    property alias changelog: changelogLabel.text
    property alias progress: progressBar.value

    signal retry()
    signal download()
    signal pause()
    signal install()

    function setError(errorString) {

    }

    height: topSlot.height + middleSlot.height + progressBarSlot.height + bottomSlot.height + divider.height

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

    Item {
        id: topSlot

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
        }

        Button {
            id: button
            anchors {
                top: update.top
                topMargin: units.gu(2)
                right: update.right
                rightMargin: units.gu(2)
            }
            height: units.gu(4)

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

    Item {
        id: middleSlot
    }

    Item {
        id: progressBarSlot
    }

    Item {
        id: bottomSlot
    }

    Label {
        id: sizeAndStatusLabel
        fontSize: "small"
        anchors {
            top: button.bottom
            topMargin: units.gu(0.5)
            right: update.right
            rightMargin: units.gu(2.2)
        }
        text: {
            return size
        }
    }

    RowLayout {
        id: expandableVersionLabel
        property bool expanded: false
        anchors {
            top: progressBar.visible ? progressBar.bottom : name.bottom
            topMargin: units.gu(0.5)
            left: iconPlaceholder.right
        }

        Label {
            id: versionLabel
            text: i18n.tr("Version %1").arg(update.version)
        }

        Icon {
            id: expandableVersionIcon
            name: "info"
            color: parent.expanded ? theme.palette.selected.baseText : theme.palette.normal.baseText
            width: units.gu(1.7)
        }

        MouseArea {
            anchors.fill: parent
            onClicked: parent.expanded = !parent.expanded
        }
    }

    ProgressBar {
        id: progressBar
        anchors {
            top: name.bottom
            topMargin: units.gu(1.1)
            left: iconPlaceholder.right
        }
        indeterminate: progress < 0 || progress > 100
        minimumValue: 0
        maximumValue: 100
        showProgressPercentage: false
        height: units.gu(0.5)
        width: units.gu(23)
        visible: {
            switch (update.mode) {
            case UpdateManager.AutomaticallyDownloading:
            case UpdateManager.ManuallyDownloading:
            case UpdateManager.DownloadPaused:
            case UpdateManager.InstallationPaused:
            case UpdateManager.Installing:
                return true;
            default:
                return false;
            }
        }
    }

    Label {
        id: changelogLabel
        visible: expandableVersionLabel.expanded
        anchors {
            top: expandableVersionLabel.bottom
            topMargin: units.gu(1)
            left: iconPlaceholder.right
            right: update.right
            rightMargin: units.gu(2)
        }
        wrapMode: Text.WordWrap
        Component.onCompleted: {
            origHeight = height;
            height = Qt.binding(function () {
                return visible ? origHeight : 0;
            });
        }
    }

    ListItems.ThinDivider {
        id: divider
        anchors {
            top: expandableVersionLabel.expanded ? changelogLabel.bottom : expandableVersionLabel.bottom
            topMargin: units.gu(1)
            left: iconPlaceholder.right
            right: update.right
            rightMargin: units.gu(2)
            bottom: update.bottom
            bottomMargin: units.gu(2)
        }
    }
}
