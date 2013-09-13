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
import Ubuntu.SystemSettings.LanguagePlugin 1.0

ItemPage {
    id: root

    title: i18n.tr("Keyboard layouts")

    UbuntuLanguagePlugin {
        id: plugin
    }

    ListView {
        id: pluginsList

        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: contentHeight > root.height ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        section.property: "subset"
        section.delegate: ListItem.Standard {
            text: section == "true" ? i18n.tr("Current layouts:") : i18n.tr("All layouts available:")
        }

        model: plugin.layoutsModel
        delegate: SettingsCheckEntry {
            textEntry: display
            checkStatus: checked

            onCheckStatusChanged: {
                var element

                if (index < plugin.layoutsModel.subset.length)
                    element = plugin.layoutsModel.subset[index]
                else
                    element = index - plugin.layoutsModel.subset.length

                plugin.layoutsModel.setChecked(element, checkStatus, checkStatus ? 0 : 2000)

                checkStatus = Qt.binding(function() { return checked })
            }
        }

        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 100 }
        }

        remove: Transition {
            ParallelAnimation {
                NumberAnimation { property: "opacity"; to: 0; duration: 100 }
                NumberAnimation { property: "y"; duration: 100 }
            }
        }

        displaced: Transition {
            NumberAnimation { property: "y"; duration: 100 }
        }
    }
}
