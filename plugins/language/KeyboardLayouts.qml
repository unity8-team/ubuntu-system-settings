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
        delegate: ListItem.Standard {
            enabled: model.enabled

            icon: Rectangle {
                width: units.gu(3.0)
                height: units.gu(3.0)
                radius: units.gu(0.5)

                color: Theme.palette.normal.backgroundText

                Label {
                    text: model.icon

                    color: Theme.palette.normal.background
                    fontSize: "small"

                    anchors.centerIn: parent
                }
            }

            text: model.language

            control: CheckBox {
                checked: model.checked

                onCheckedChanged: {
                    var element = index < subsetView.model.subset.length ?
                                  subsetView.model.subset[index] :
                                  index - subsetView.model.subset.length

                    plugin.keyboardLayoutsModel.setChecked(element, checked, checked ? 0 : subsetView.delay)

                    checked = Qt.binding(function() { return model.checked })
                }
            }
        }
    }
}
