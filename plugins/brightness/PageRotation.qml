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
    objectName: "resolutionPage"

    property var orientation

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
                id: orientationSelector
                objectName: "orientationSelector"
                model: [
                    i18n.tr("None"),
                    i18n.tr("90° clockwise"),
                    i18n.tr("180° clockwise"),
                    i18n.tr("270° clockwise")
                ]
                expanded: true
                highlightWhenPressed: false
                onDelegateClicked: {
                    switch (index) {
                        case 0:
                            orientation = Display.Normal;
                            break;
                        case 1:
                            orientation = Display.PortraitMode;
                            break;
                        case 2:
                            orientation = Display.LandscapeInvertedMode;
                            break;
                        case 3:
                            orientation = Display.PortraitInvertedMode;
                            break;
                    }
                }
                Component.onCompleted: {
                    console.warn('orientation', orientation)
                    switch (orientation) {
                        case Display.Normal:
                            selectedIndex = 0;
                            break;
                        case Display.PortraitMode:
                            selectedIndex = 1;
                            break;
                        case Display.LandscapeInvertedMode:
                            selectedIndex = 2;
                            break;
                        case Display.PortraitInvertedMode:
                            selectedIndex = 3;
                            break;
                        default:
                            throw "Unable to determine orientation type.";
                    }
                }
            }
        }
    }
}
