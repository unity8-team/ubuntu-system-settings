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

Column {
    id: updates
    signal requestedRetry(string packageName)
    signal requestDownload(int udmDownloadId)
    height: rep.contentHeight
    property alias model: rep.model
    Repeater {
        anchors { left: parent.left; right: parent.right }
        id: rep
        delegate: ClickUpdate {
            anchors { left: parent.left; right: parent.right }
            onRequestedRetry: updates.requestedRetry(packageName)
            formatter: Utilities.formatSize
        }
    }
}
