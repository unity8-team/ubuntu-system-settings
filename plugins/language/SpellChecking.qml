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
import GSettings 1.0
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.SystemSettings.LanguagePlugin 1.0

ItemPage {
    title: i18n.tr("Spell checking")
    flickable: scrollWidget

    UbuntuLanguagePlugin {
        id: plugin
    }

    GSettings {
        id: settings

        schema.id: "com.canonical.keyboard.maliit"
    }

    ListItem.Standard {
        id: item

        text: i18n.tr("Spell checking")

        control: Switch {
            property bool serverChecked: settings.spellChecking
            onServerCheckedChanged: checked = serverChecked
            Component.onCompleted: checked = serverChecked
            onTriggered: settings.spellChecking = checked
        }
    }

    SubsetView {
        id: scrollWidget
        clip: true

        anchors.top: item.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        subsetLabel: i18n.tr("Current spelling languages:")
        supersetLabel: i18n.tr("All languages available:")

        model: plugin.spellCheckingModel
    }
}
