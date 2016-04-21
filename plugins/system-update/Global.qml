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
 */

import QtQuick 2.4
import QtQuick.Layouts 1.2
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.Update 1.0

Item {
    id: g
    property int managerStatus // A UpdateManager::ManagerStatus
    property bool requireRestart: false
    property int updatesCount: 0
    property bool online: false

    // Indicate whether or not this component should be displayed
    property bool hidden: updatesCount === 1 && managerStatus === UpdateManager.Idle

    signal stop()
    signal pause()
    signal install()

    clip: true

    Behavior on height {
        UbuntuNumberAnimation {}
    }

    RowLayout {
        id: checking
        spacing: units.gu(2)
        anchors.fill: parent
        Behavior on opacity {
            UbuntuNumberAnimation {}
        }
        opacity: visible ? 1 : 0
        visible: {
            switch (g.managerStatus) {
            case UpdateManager.CheckingClickUpdates:
            case UpdateManager.CheckingSystemUpdates:
            case UpdateManager.CheckingAllUpdates:
                return true;
            }
            return false;
        }

        ActivityIndicator {
            running: parent.visible
        }

        Label {
            text: i18n.tr("Checking for updates…")
        }

        Button {
            text: i18n.tr("Stop")
            onClicked: g.stop()
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        }
    }

    RowLayout {
        id: install
        anchors.fill: parent
        Behavior on opacity {
            UbuntuNumberAnimation {}
        }
        opacity: visible ? 1 : 0
        visible: {
            var canInstall = g.managerStatus === UpdateManager.Idle;
            return canInstall && updatesCount > 1;
        }

        Button {
            text: {
                if (g.requireRestart) {
                    return i18n.tr(
                        "Install %1 update…",
                        "Install %1 updates…",
                        updatesCount
                    ).arg(updatesCount);
                } else {
                    return i18n.tr(
                        "Install %1 update",
                        "Install %1 updates",
                        updatesCount
                    ).arg(updatesCount);
                }
            }
            onClicked: g.install()
            Layout.fillWidth: true
        }
    }

    RowLayout {
        id: pause
        anchors.fill: parent
        visible: false

        Button {
            text: i18n.tr("Pause All")
            onClicked: g.pause()
            Layout.fillWidth: true
        }
    }
}
