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
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
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

    height: childrenRect.height

    Item {
        id: iconPlaceholder
        height: update.height
        width: units.gu(9)

        anchors {
            top: update.top
            left: update.left
            bottom: update.bottom
        }

        Icon {
            id: icon
            anchors {
                margins: units.gu(2)
                top: iconPlaceholder.top
                left: iconPlaceholder.left
            }
            width: iconPlaceholder.width - (units.gu(2) * 2)
        }
    }

    Label {
        id: name
        anchors {
            left: iconPlaceholder.right
            top: update.top
            topMargin: units.gu(2)
        }
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
        // Enabled as long as it's not NonPausable.
        // enabled: update.mode !== updateManager.nonPausable

        text: {
            console.warn("update.mode", update.mode)
            switch(update.mode) {
            case UM.Retriable:
                return i18n.tr("Retry")
            case UM.Downloadable:
                return i18n.tr("Download")
            case UM.Pausable:
            case UM.NonPausable:
                return i18n.tr("Pause")
            case UM.Installable:
                return i18n.tr("Install")
            case UM.InstallableWithRestart:
                return i18n.tr("Install…")
            default:
                return "FUUUUU"
            }
        }

        onClicked: {
            switch (update.mode) {
            case UM.Retriable:
                update.retry();
                break;
            case UM.Downloadable:
                update.download();
                break;
            case UM.Pausable:
                update.pause();
                break;
            case UM.Installable:
            case UM.InstallableWithRestart:
                update.install();
                break;
            case UM.NonPausable:
                break;
            }
        }
    }

    Label {
        id: sizeAndStatusLabel
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

    Label {
        id: versionLabel
        anchors {
            top: name.bottom
            topMargin: units.gu(1)
            left: iconPlaceholder.right

        }
        text: i18n.tr("Version %1").arg(update.version)
    }

    ProgressBar {
        id: progressBar
        indeterminate: progress < 0 || progress > 100
        minimumValue: 0
        maximumValue: 100
        showProgressPercentage: false
        height: units.gu(0.5)
        visible: false
    }

    Label {

    }

    ListItems.Expandable {
        id: changelogLabel
        anchors {
            left: iconPlaceholder.right
            bottom: update.bottom
            bottomMargin: units.gu(2)
        }
        collapsedHeight: 1
        expandedHeight: units.gu(30)
        onClicked: {
            expanded = true;
        }
    }
}
