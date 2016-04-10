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

Item {
    id: update

    property alias name: name.text
    property int status // This is an updateManager::UpdateStatus
    property int mode // This is an updateManager::UpdateMode
    property string version
    property alias iconUrl: icon.source
    property string size
    property string changelog
    property int progress // From 0 to 100

    signal retry()
    signal download()
    signal pause()
    signal install()

    // MOCK
    property var updateManager: ({
        "downloadable": 0,
        "installable": 1,
        "installableWithRestart": 2,
        "pausable": 3,
        "nonPausable": 4,
        "retriable": 5,
    })

    function setError(errorString) {

    }

    states: [
    ]

    Icon {
        id: icon
    }

    Label {
        id: name
        font.bold: true
        elide: Text.ElideMiddle
    }

    Button {
        // Enabled as long as it's not NonPausable.
        enabled: update.mode !== updateManager.nonPausable

        text: {
            switch(update.mode) {
            case updateManager.retriable:
                return i18n.tr("Retry")
            case updateManager.downloadable:
                return i18n.tr("Download")
            case updateManager.pausable:
            case updateManager.nonPausable:
                return i18n.tr("Pause")
            case updateManager.installable:
                return i18n.tr("Install")
            case updateManager.installableWithRestart:
                return i18n.tr("Install…")
            }
        }

        onClicked: {
            switch (update.mode) {
            case updateManager.retriable:
                update.retry();
                break;
            case updateManager.downloadable:
                update.download();
                break;
            case updateManager.pausable:
                update.pause();
                break;
            case updateManager.installable:
            case updateManager.installableWithRestart:
                update.install();
                break;
            case updateManager.nonPausable:
                break;
            }
        }
    }
}
