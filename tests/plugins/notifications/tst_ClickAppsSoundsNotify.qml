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

ClickAppsSoundsNotify {
    id: root

    width: 300
    height: 500

    model: ClickApplicationsNotifyModel {
        id: clickAppsSoundsNotifyModel
        notifyType: ClickApplicationsNotifyModel.SoundsNotify
        sourceModel: ClickApplicationsModel
    }

    TestCase {
        name: "ClickAppsSoundsNotify"
        when: windowShown

        function init() {
            ClickApplicationsModel.cleanup()
        }

        function test_start_empty() {
            compare(clickAppsSoundsNotifyModel.count, 0)

            var appsList = findChild(root, "appsSoundsNotifyList")
            compare(appsList.count, 0)
        }

        function test_should_update_when_added() {
            compare(clickAppsSoundsNotifyModel.count, 0)

            ClickApplicationsModel.addApplication("Pkg01", "App01")

            clickAppsSoundsNotifyModel.updateEnabledEntries()
            compare(clickAppsSoundsNotifyModel.count, 1)

            var appsList = findChild(root, "appsSoundsNotifyList")
            compare(appsList.count, 1)
        }

        function test_should_update_when_removed() {
            ClickApplicationsModel.addApplication("Pkg01", "App01")
            clickAppsSoundsNotifyModel.updateEnabledEntries()
            compare(clickAppsSoundsNotifyModel.count, 1)

            ClickApplicationsModel.removeApplicationByIndex(0)
            clickAppsSoundsNotifyModel.updateEnabledEntries()
            compare(clickAppsSoundsNotifyModel.count, 0)

            var appsList = findChild(root, "appsSoundsNotifyList")
            compare(appsList.count, 0)
        }
    }
}
