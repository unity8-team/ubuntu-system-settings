/*
 * Copyright 2017 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by Jonas G. Drange <jonas.drange@canonical.com>
 */

import QtQuick 2.4
import QtQuick.Layouts 1.3
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems

Column {
    height: childrenRect.height

    TextBoxListItem {
        id: description
        text: i18n.tr("Description")
        field.text: printer.description
        onFieldTextChanged: printer.description = fieldText
    }

    ListItems.ValueSelector {
        anchors {
            left: parent.left
            right: parent.right
        }
        enabled: values.length > 1
        text: i18n.tr("Duplex")
        values: printer.supportedDuplexModes
        onSelectedIndexChanged: printer.duplexMode = selectedIndex
        Component.onCompleted: {
            if (enabled) {
                selectedIndex = printer.duplexMode
            }
        }
    }

    ListItems.ValueSelector {
        anchors {
            left: parent.left
            right: parent.right
        }
        visible: printer.supportedColorModels.length
        text: i18n.tr("Color model")
        values: printer.supportedColorModels
        enabled: values.length > 1
        onSelectedIndexChanged: printer.colorModel = selectedIndex
        Component.onCompleted: {
            if (enabled)
                selectedIndex = printer.colorModel
        }
    }

    ListItems.ValueSelector {
        anchors {
            left: parent.left
            right: parent.right
        }
        visible: printer.supportedPrintQualities.length
        text: i18n.tr("Quality")
        values: printer.supportedPrintQualities
        enabled: values.length > 1
        onSelectedIndexChanged: printer.printQuality = selectedIndex
        Component.onCompleted: {
            if (enabled)
                selectedIndex = printer.printQuality
        }
    }
}
