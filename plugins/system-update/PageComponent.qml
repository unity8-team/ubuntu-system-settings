/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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
import Ubuntu.SystemSettings.Update 1.0


ItemPage {
    id: root

    title: i18n.tr("Update phone")
    flickable: scrollWidget // maybe remove

    UbuntuUpdatePanel {
        id: updateID

        function statusMessage() {
            if (updateAvailable === 0)
                return i18n.tr("Congrats! You are already up to date!");
            else if (updateAvailable === 1)
                return i18n.tr("New version is available, click for more details");
            return i18n.tr("Checking latest available system version…");
        }
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds



        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Standard {
                text: updateID.statusMessage();
            }
            ListItem.Standard {
                text: "OS version " + updateID.OSVersion;
            }
            ListItem.Standard {
                text: "Test " + updateID.updateSize;
            }
        }
    }

}
