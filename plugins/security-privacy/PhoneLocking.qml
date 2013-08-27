/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

ItemPage {
    title: i18n.tr("Phone locking")

    Column {
        anchors.left: parent.left
        anchors.right: parent.right

        ListItem.SingleValue {
            text: i18n.tr("Lock security")
            value: i18n.tr("Swipe (no security)")
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("LockSecurity.qml"))
        }

        ListItem.SingleValue {
            text: i18n.tr("Lock when idle")
            value: i18n.tr("1 minute",
                           "%1 minutes".arg(5),
                           5)
            progression: true
            onClicked: pageStack.push(Qt.resolvedUrl("LockWhenIdle.qml"))
        }

        SettingsCheckEntry {
            checkStatus: true
            textEntry: i18n.tr("Sleep locks immediately")
            enabled: false /* TODO: enable when there is a backend */
        }

    /* TODO: once we know how to do this
    ListItem.Standard {
        text: i18n.tr("When locked, allow:")
    }
    Launcher,
    Camera,
    ...
    */
    }
}
