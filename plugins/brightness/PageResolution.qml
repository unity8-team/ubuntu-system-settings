/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
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
 *
 * Authors:
 *      Jonas G. Drange <jonas.drange@canonical.com>
 */

import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.SystemSettings.Brightness 1.0

ItemPage {
    id: root
    title: i18n.tr("Resolution")
    objectName: "modePage"

    property var modes
    property string mode

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
                        Flickable.DragAndOvershootBounds :
                        Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.ItemSelector {
                id: modeSelector
                objectName: "modeSelector"
                delegate: OptionSelectorDelegate {
                    text: modelData
                }
                model: modes
                expanded: true
                highlightWhenPressed: false
                onDelegateClicked: mode = model[index]
                Component.onCompleted: {
                    selectedIndex = modes.indexOf(mode)
                    console.warn("slected", modes, mode, modes.indexOf(mode))
                }
            }
        }
    }
}
