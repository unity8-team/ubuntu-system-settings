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
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.SystemSettings.Update 1.0

Item {
    id: g

    property int status // A SystemUpdate::Status
    property bool requireRestart: false
    property int updatesCount: 0
    property bool online: false

    property bool hidden: {
        // For some, we hide unconditionally:
        switch (status) {
        case SystemUpdate.StatusNetworkError:
        case SystemUpdate.StatusServerError:
            return true;
        }
        return updatesCount <= 1 && status === SystemUpdate.StatusIdle || !online;
    }

    signal stop()
    signal pause()
    signal requestInstall()
    signal install()
    signal resume()

    // clip: true

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
            switch (g.status) {
            case SystemUpdate.StatusCheckingClickUpdates:
            case SystemUpdate.StatusCheckingSystemUpdates:
            case SystemUpdate.StatusCheckingAllUpdates:
                return true;
            }
            return false;
        }

        ActivityIndicator {
            running: parent.visible
            Layout.leftMargin: units.gu(2)
        }

        Label {
            text: i18n.tr("Checking for updates…")
        }

        Button {
            objectName: "updatesGlobalStopButton"
            text: i18n.tr("Stop")
            onClicked: g.stop()
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.rightMargin: units.gu(2)
        }
    }

    RowLayout {
        id: install
        anchors.fill: parent
        spacing: units.gu(2)
        Behavior on opacity {
            UbuntuNumberAnimation {}
        }
        opacity: visible ? 1 : 0
        visible: {
            var canInstall = g.status === SystemUpdate.StatusIdle;
            return canInstall && updatesCount > 1;
        }

        Label {
            objectName: "updatesGlobalInfoLabel"
            // TRANSLATORS: %1 is number of software updates available.
            text: i18n.tr("%1 update available",
                          "%1 updates available",
                          updatesCount).arg(updatesCount)
            Layout.fillWidth: true
            Layout.leftMargin: units.gu(2)
        }

        Button {
            objectName: "updatesGlobalInstallButton"
            text: {
                if (g.requireRestart === true) {
                    return i18n.tr("Update all…");
                } else {
                    return i18n.tr("Update all");
                }
            }
            onClicked: g.requestInstall()
            color: theme.palette.normal.positive
            strokeColor: "transparent"
            Layout.rightMargin: units.gu(2)
        }
    }

    Button {
        id: pause
        objectName: "updatesGlobalPauseButton"
        text: i18n.tr("Pause All")
        anchors {
            fill: parent
            margins: units.gu(2)
        }
        visible: {
            var batchMode = g.status === SystemUpdate.StatusBatchMode;
            return batchMode && updatesCount >= 1;
        }
        onClicked: g.pause()
    }

    Button {
        id: resume

        anchors {
            fill: parent
            margins: units.gu(2)
        }

        objectName: "updatesGlobalResumeButton"
        text: i18n.tr("Resume All")
        visible: {
            var batchModePaused = g.status === SystemUpdate.StatusBatchModePaused;
            return batchModePaused && updatesCount >= 1;
        }
        onClicked: g.resume()
    }

    ListItems.ThinDivider {
        visible: !g.hidden
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
}
