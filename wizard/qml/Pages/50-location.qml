/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.3
import QMenuModel 0.1 as QMenuModel
import Qt.labs.folderlistmodel 2.1
import Ubuntu.Components 1.1
import Ubuntu.SystemSettings.Wizard.Utils 0.1
import "../Components" as LocalComponents

LocalComponents.Page {
    title: i18n.tr("Location")
    forwardButtonSourceComponent: forwardButton

    FolderListModel {
        id: termsModel
        folder: System.hereLicensePath
        nameFilters: ["*.html"]
        showDirs: false
        showOnlyReadable: true
    }

    QMenuModel.QDBusActionGroup {
        id: locationActionGroup
        busType: QMenuModel.DBus.SessionBus
        busName: "com.canonical.indicator.location"
        objectPath: "/com/canonical/indicator/location"
        property variant enabled: action("location-detection-enabled")
        Component.onCompleted: start()
    }

    Column {
        id: column
        anchors.fill: content
        spacing: units.gu(2)

        LocalComponents.CheckableSetting {
            id: locationCheck
            showDivider: false
            text: i18n.tr("Use your mobile network and Wi-Fi to determine where you are")
            checked: locationActionGroup.enabled.state
            onTriggered: locationActionGroup.enabled.activate()
        }

        LocalComponents.CheckableSetting {
            id: termsCheck
            showDivider: false
            visible: System.hereLicensePath !== "" && termsModel.count > 0
            // TRANSLATORS: HERE is a trademark for Nokia's location service, you probably shouldn't translate it
            text: i18n.tr("I have read and agreed to the HERE <a href='terms.qml'>Terms and Conditions</a>")
            onLinkActivated: pageStack.load(Qt.resolvedUrl(link))
            checked: System.hereEnabled
            onTriggered: System.hereEnabled = checked
        }
    }

    Component {
        id: forwardButton
        LocalComponents.StackButton {
            text: i18n.tr("Continue")
            onClicked: pageStack.next()
        }
    }
}
