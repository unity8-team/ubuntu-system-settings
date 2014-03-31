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

import QtQuick 2.0
import QMenuModel 0.1
import Ubuntu.Components 0.1
import Ubuntu.SystemSettings.Diagnostics 1.0
import "../Components" as LocalComponents

LocalComponents.Page {
    id: userSettingsPage
    title: i18n.tr("Phone settings")
    forwardButtonSourceComponent: forwardButton

    UbuntuDiagnostics {
        id: diagnosticsWidget
        onReportCrashesChanged: reportCrashesCheck.checked = diagnosticsWidget.canReportCrashes
    }

    Item {
        id: content
        anchors {
            fill: parent
            bottomMargin: userSettingsPage.bottomMargin
        }

        QDBusActionGroup {
            id: locationActionGroup
            busType: DBus.SessionBus
            busName: "com.canonical.indicator.location"
            objectPath: "/com/canonical/indicator/location"

            property variant enabled: action("location-detection-enabled")

            Component.onCompleted: start()
        }

        Binding {
            target: locationOn
            property: "checked"
            value: locationActionGroup.enabled.state
        }

        Column {
            anchors.fill: parent

            LocalComponents.SectionHeader {
                id: userExperienceSectionHeader
                width: parent.width
                leftMargin: userSettingsPage.leftMargin
                rightMargin: userSettingsPage.rightMargin
                text: i18n.tr("User experience")
            }

            LocalComponents.CheckableSetting {
                id: reportCrashesCheck
                leftMargin: userSettingsPage.leftMargin
                rightMargin: userSettingsPage.rightMargin
                text: i18n.tr("Your phone is setup to automatically report errors to Canonical. This can be disabled in system settings.")
                checked: diagnosticsWidget.canReportCrashes
                onCheckedChanged: diagnosticsWidget.canReportCrashes = checked;
            }

            LocalComponents.SectionHeader {
                id: locationServicesSectionHeader
                width: parent.width
                leftMargin: userSettingsPage.leftMargin
                rightMargin: userSettingsPage.rightMargin
                text: i18n.tr("Location services")
            }

            LocalComponents.CheckableSetting {
                id: locationOn
                leftMargin: userSettingsPage.leftMargin
                rightMargin: userSettingsPage.rightMargin
                text: i18n.tr("Your phone is setup to detect your location. This can be disabled in system settings.")
                checked:  locationActionGroup.enabled.state
                onClicked: locationActionGroup.enabled.activate()
            }
        }
    }

    Component {
        id: forwardButton
        LocalComponents.ForwardButton {
            text: i18n.tr("Continue")
            onClicked: pageStack.next()
        }
    }
}
