/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
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
import Ubuntu.Components.ListItems 0.1 as ListItem

ListView {
    id: root

    property int delay: 2000
    property int duration: 100

    property string subsetLabel
    property string supersetLabel

    contentHeight: contentItem.childrenRect.height
    boundsBehavior: contentHeight > height ?
                    Flickable.DragAndOvershootBounds :
                    Flickable.StopAtBounds

    section.property: "subset"
    section.delegate: ListItem.Standard {
        text: section == "true" ? subsetLabel : supersetLabel
    }

    delegate: SettingsCheckEntry {
        textEntry: display
        checkStatus: checked

        onCheckStatusChanged: {
            var element = index < root.model.subset.length ? root.model.subset[index] : index - root.model.subset.length

            root.model.setChecked(element, checkStatus, checkStatus ? 0 : delay)

            checkStatus = Qt.binding(function() { return checked })
        }
    }

    add: Transition {
        NumberAnimation {
            property: "opacity"
            duration: duration
            from: 0
            to: 1
        }
    }

    displaced: Transition {
        NumberAnimation {
            property: "y"
            duration: duration
        }
    }

    remove: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                duration: duration
                to: 0
            }

            NumberAnimation {
                property: "y"
                duration: duration
            }
        }
    }
}
