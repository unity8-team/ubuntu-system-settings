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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import SystemSettings 1.0

ItemPage {
    id: root

    title: i18n.tr("Lock when idle")
    flickable: scrollWidget

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Standard {
                text: i18n.tr("Lock the phone when it is not in use:")
            }

            ListItem.ValueSelector {
                id: sleepSelector
                expanded: true
                // TODO: There is no way to have a ValueSelector always expanded
                onExpandedChanged: expanded = true
                values: [i18n.tr("After %1 minutes").arg(1),
                    i18n.tr("After %1 minute").arg(2),
                    i18n.tr("After %1 minutes").arg(3),
                    i18n.tr("After %1 minutes").arg(4),
                    i18n.tr("After %1 minutes").arg(5),
                    i18n.tr("After %1 minutes").arg(10),
                    i18n.tr("Never")]
                selectedIndex: 4 // 5
            }

            ListItem.Caption {
                text: i18n.tr("Shorter times are more secure. Phone won't lock during calls or video playback.")
            }
        }
    }
}
