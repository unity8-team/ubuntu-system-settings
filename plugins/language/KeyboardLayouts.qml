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
import Ubuntu.Components 0.1
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

        section.property: "section"
        section.delegate: ListItem.Standard {
            text: section == "subset" ? i18n.tr("Current layouts:") : i18n.tr("All layouts available:")
        }

        model: plugin.pluginsModel
        delegate: SettingsCheckEntry {
            textEntry: label
            checkStatus: checked

            onClicked: plugin.pluginsModel.setInSubset(index < plugin.pluginsModel.subset.length ?
                                                       plugin.pluginsModel.subset[index] :
                                                       index - plugin.pluginsModel.subset.length,
                                                       checkStatus)
        }
    }
}
