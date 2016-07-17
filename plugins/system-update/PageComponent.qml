/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
 *
 * Contact: Didier Roche <didier.roches@canonical.com>
 *          Diego Sarmentero <diego.sarmentero@canonical.com>
 *          Jonas G. Drange <jonas.drange@canonical.com>
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
import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Connectivity 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.Components.Popups 1.3
import Ubuntu.OnlineAccounts.Client 0.1
import Ubuntu.SystemSettings.Update 1.0

ItemPage {
    id: root
    objectName: "systemUpdatesPage"
    flickable: updates.flickable

    QDBusActionGroup {
        id: indicatorPower
        busType: 1
        busName: "com.canonical.indicator.power"
        objectPath: "/com/canonical/indicator/power"
        property variant batteryLevel: action("battery-level").state
        property variant deviceState: action("device-state").state
        Component.onCompleted: start()
    }

    ClickManager {
        id: clickManager
    }

    Setup {
        id: uoaConfig
        applicationId: "ubuntu-system-settings"
        providerId: "ubuntuone"

        onFinished: {
            if (reply.errorName)
                console.warn('Online Accounts failed:', reply.errorName);
            else
                updates.checkClick();
        }
    }

    UpdateModelFilter {
        id: imageUpdate
        kindFilter: Update.KindImage
        installed: false
    }

    UpdateModelFilter {
        id: clickUpdates
        kindFilter: Update.KindClick
        installed: false
    }

    UpdateModelFilter {
        id: installedUpdates
        installed: true
    }

    DownloadHandler {
        id: downloadHandler
        updateModel: SystemUpdate.model
    }

    Updates {
        id: updates
        anchors {
            top: parent.top
            bottom: configuration.top
        }
        width: parent.width
        clip: true

        clickModel: clickUpdates
        clickManager: clickManager
        imageModel: imageUpdate
        installedModel: installedUpdates
        downloadHandler: downloadHandler

        authenticated: clickManager.authenticated

        havePower: (indicatorPower.deviceState === "charging")
                    && (indicatorPower.batteryLevel > 25)
        onRequestAuthentication: uoaConfig.exec()
    }

    Column {
        id: configuration

        height: childrenRect.height

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        ListItem.ThinDivider {}

        ListItem.SingleValue {
            objectName: "configuration"
            text: i18n.tr("Auto download")
            value: {
                if (SystemImage.downloadMode === 0)
                    return i18n.tr("Never")
                else if (SystemImage.downloadMode === 1)
                    return i18n.tr("On wi-fi")
                else if (SystemImage.downloadMode === 2)
                    return i18n.tr("Always")
            }
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("Configuration.qml"))
        }
    }

    Component {
         id: dialogErrorComponent
         Dialog {
             id: dialogueError
             title: i18n.tr("Installation failed")
             text: root.errorDialogText

             Button {
                 text: i18n.tr("OK")
                 color: UbuntuColors.orange
                 onClicked: {
                     PopupUtils.close(dialogueError);
                 }
             }
         }
    }
}
