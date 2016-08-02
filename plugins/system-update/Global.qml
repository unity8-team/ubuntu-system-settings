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

    property bool batchMode
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
        return !online ||
               (updatesCount <= 1 && status === SystemUpdate.StatusIdle)  ||
               batchMode
    }

    signal stop()
    signal requestInstall()
    signal install()

    Behavior on height {
        UbuntuNumberAnimation {}
    }

    RowLayout {
        id: checking
        spacing: units.gu(2)
        anchors {
            fill: parent
            margins: units.gu(2)
        }

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
        }

        Label {
            text: i18n.tr("Checking for updates…")
        }

        Button {
            objectName: "updatesGlobalStopButton"
            // TODO: String should be “Stop”
            text: i18n.tr("Cancel")
            onClicked: g.stop()
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        }
    }

    RowLayout {
        id: install
        anchors {
            fill: parent
            margins: units.gu(2)
        }
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
            visible: showAllUI // Hidden due to missing strings
        }

        Button {
            objectName: "updatesGlobalInstallButton"
            text: {
                if (g.requireRestart === true) {
                    // TODO: String should be “Update all…”
                    return i18n.tr("Restart & Install");
                } else {
                    // TODO: String should be “Update all”
                    return i18n.tr("Install");
                }
            }
            onClicked: g.requestInstall()
            color: theme.palette.normal.positive
            strokeColor: "transparent"
        }
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
