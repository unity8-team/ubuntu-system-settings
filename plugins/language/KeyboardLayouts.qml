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

import SystemSettings 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.SystemSettings.LanguagePlugin 1.0

ItemPage {
    title: i18n.tr("Keyboard layouts")

    UbuntuLanguagePlugin {
        id: plugin
    }

    SubsetView {
        id: subsetView

        clip: true

        anchors.fill: parent

        subsetLabel: i18n.tr("Current layouts:")
        supersetLabel: i18n.tr("All layouts available:")

        model: plugin.keyboardLayoutsModel
        delegate: KeyboardLayoutItem {
            name: model.language
            shortName: model.icon
            checked: model.checked
            enabled: model.enabled

            onCheckedChanged: {
                var element = model.index < subsetView.model.subset.length ?
                              subsetView.model.subset[model.index] :
                              model.index - subsetView.model.subset.length

                plugin.keyboardLayoutsModel.setChecked(element, checked, checked ? 0 : subsetView.delay)

                checked = Qt.binding(function() { return model.checked })
            }
        }
    }
}
