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

import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

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
    /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
       otherwise the UI might end up in a situation where scrolling doesn't work */
    flickableDirection: Flickable.VerticalFlick

    section.property: "subset"
    section.delegate: ListItem.Standard {
        text: section == "true" ? subsetLabel : supersetLabel
    }

    delegate: ListItem.Standard {
        text: model.display
        control: CheckBox {
            checked: model.checked
            onCheckedChanged: {
                var element = model.index < root.model.subset.length ?
                              root.model.subset[model.index] :
                              model.index - root.model.subset.length

                root.model.setChecked(element, checked, checked ? 0 : delay)

                checked = Qt.binding(function() { return model.checked })
            }
        }
        enabled: model.enabled
    }
}
