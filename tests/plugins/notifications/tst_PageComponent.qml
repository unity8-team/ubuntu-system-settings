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
import QtTest 1.0
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.Notifications 1.0 

import NotificationsSource 1.0

PageComponent {
    id: root

    width: 300
    height: 500

    TestCase {
        name: "NotificationsPageComponent"
        when: windowShown

        function cleanup() {
            ClickApplicationsModel.cleanup()
        }

        function test_start_empty() {
            var soundsModel = findChild(root, "clickAppsSoundsNotifyModel")
            compare(soundsModel.count, 0)

            var vibrationsModel = findChild(root, "clickAppsVibrationsNotifyModel")
            compare(vibrationsModel.count, 0)

            var appsList = findChild(root, "notificationsList")
            compare(appsList.count, 0)
        }

        function test_updated_when_added() {
            var soundsModel = findChild(root, "clickAppsSoundsNotifyModel")
            compare(soundsModel.count, 0)

            var vibrationsModel = findChild(root, "clickAppsVibrationsNotifyModel")
            compare(vibrationsModel.count, 0)

            var appsList = findChild(root, "notificationsList")
            compare(appsList.count, 0)

            ClickApplicationsModel.addApplication("Pkg01", "App01")
            ClickApplicationsModel.addApplication("Pkg02", "App02")

            compare(soundsModel.count, 2)
            compare(vibrationsModel.count, 2)
            compare(appsList.count, 2)

            var soundsLabel = findChild(root, "clickAppsSoundsNotifyLabel")
            tryCompare(soundsLabel, "text", "2")

            var vibrationsLabel = findChild(root, "clickAppsVibrationsNotifyLabel")
            tryCompare(vibrationsLabel, "text", "2")
        }

        function test_updated_when_removed() {
            ClickApplicationsModel.addApplication("Pkg01", "App01")
            ClickApplicationsModel.addApplication("Pkg02", "App02")

            var soundsModel = findChild(root, "clickAppsSoundsNotifyModel")
            compare(soundsModel.count, 2)

            var vibrationsModel = findChild(root, "clickAppsVibrationsNotifyModel")
            compare(vibrationsModel.count, 2)

            var appsList = findChild(root, "notificationsList")
            compare(appsList.count, 2)

            ClickApplicationsModel.removeApplicationByIndex(0)

            compare(soundsModel.count, 1)
            compare(vibrationsModel.count, 1)
            compare(appsList.count, 1)

            var soundsLabel = findChild(root, "clickAppsSoundsNotifyLabel")
            tryCompare(soundsLabel, "text", "1")

            var vibrationsLabel = findChild(root, "clickAppsVibrationsNotifyLabel")
            tryCompare(vibrationsLabel, "text", "1")
        }

        function test_sounds_filter_is_updated() {
            var soundsModel = findChild(root, "clickAppsSoundsNotifyModel")
            compare(soundsModel.count, 0)

            var soundsLabel = findChild(root, "clickAppsSoundsNotifyLabel")

            ClickApplicationsModel.addApplication("Pkg01", "App01")
            ClickApplicationsModel.addApplication("Pkg02", "App02")
            ClickApplicationsModel.addApplication("Pkg03", "App03")

            soundsModel.updateEnabledEntries()
            compare(soundsModel.count, 3)
            tryCompare(soundsLabel, "text", "3")

            ClickApplicationsModel.setNotificationByIndex(ClickApplicationsModel.EnableNotifications, 0, false)

            soundsModel.updateEnabledEntries()
            compare(soundsModel.count, 2)
            tryCompare(soundsLabel, "text", "2")

            ClickApplicationsModel.setNotificationByIndex(ClickApplicationsModel.SoundsNotify, 1, false)

            soundsModel.updateEnabledEntries()
            compare(soundsModel.count, 1)
            tryCompare(soundsLabel, "text", "1")

            ClickApplicationsModel.setNotificationByIndex(ClickApplicationsModel.VibrationsNotify, 2, false)

            soundsModel.updateEnabledEntries()
            compare(soundsModel.count, 1)
            tryCompare(soundsLabel, "text", "1")
        }

        function test_vibrations_filter_is_updated() {
            var vibrationsModel = findChild(root, "clickAppsVibrationsNotifyModel")
            compare(vibrationsModel.count, 0)

            var vibrationsLabel = findChild(root, "clickAppsVibrationsNotifyLabel")

            ClickApplicationsModel.addApplication("Pkg01", "App01")
            ClickApplicationsModel.addApplication("Pkg02", "App02")
            ClickApplicationsModel.addApplication("Pkg03", "App03")

            vibrationsModel.updateEnabledEntries()
            compare(vibrationsModel.count, 3)
            tryCompare(vibrationsLabel, "text", "3")

            ClickApplicationsModel.setNotificationByIndex(ClickApplicationsModel.EnableNotifications, 0, false)

            vibrationsModel.updateEnabledEntries()
            compare(vibrationsModel.count, 2)
            tryCompare(vibrationsLabel, "text", "2")

            ClickApplicationsModel.setNotificationByIndex(ClickApplicationsModel.VibrationsNotify, 1, false)

            vibrationsModel.updateEnabledEntries()
            compare(vibrationsModel.count, 1)
            tryCompare(vibrationsLabel, "text", "1")

            ClickApplicationsModel.setNotificationByIndex(ClickApplicationsModel.SoundsNotify, 2, false)

            vibrationsModel.updateEnabledEntries()
            compare(vibrationsModel.count, 1)
            tryCompare(vibrationsLabel, "text", "1")
        }
    }
}
