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
import QMenuModel 0.1
import Ubuntu.Components 1.1
import "../Components" as LocalComponents

LocalComponents.Page {
    title: i18n.tr("Location")
    forwardButtonSourceComponent: forwardButton

    QDBusActionGroup {
        id: locationActionGroup
        busType: DBus.SessionBus
        busName: "com.canonical.indicator.location"
        objectPath: "/com/canonical/indicator/location"

        property variant enabled: action("location-detection-enabled")

        Component.onCompleted: start()
    }

    Column {
        id: content
        anchors {
            fill: parent
            topMargin: topMargin
            leftMargin: leftMargin
            rightMargin: rightMargin
            bottomMargin: bottomMargin
        }
        spacing: units.gu(4)

        LocalComponents.CheckableSetting {
            id: locationCheck
            showDivider: false
            text: i18n.tr("Use your mobile network and wi-fi to work out where you are.")
            checked: locationActionGroup.enabled.state
            onTriggered: locationActionGroup.enabled.activate()
        }

        LocalComponents.CheckableSetting {
            id: termsCheck
            showDivider: false
            text: i18n.tr("I have read and agreed to the Ubuntu <a href='terms.qml'>terms and conditions</a>.")
            onLinkActivated: pageStack.push(Qt.resolvedUrl(link))
        }
    }

    Component {
        id: forwardButton
        LocalComponents.ForwardButton {
            text: i18n.tr("Continue")
            onClicked: pageStack.next()
            enabled: termsCheck.checked
        }
    }
}
