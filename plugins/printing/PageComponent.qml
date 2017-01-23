/*
 * This file is part of system-settings
 *
 * Copyright (C) 2017 Canonical Ltd.
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
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Settings.Printers 0.1

ItemPage {
    id: root
    objectName: "printingPage"

    title: i18n.tr("Printing")
    flickable: printerList

    ListView {
        id: printerList
        anchors { fill: parent }
        model: Printers.allPrinters
        delegate: ListItem {
            height: modelLayout.height + (divider.visible ? divider.height : 0)
            ListItemLayout {
                id: modelLayout
                title.text: displayName
                title.font.bold: model.default
                subtitle.text: description

                Icon {
                    id: icon
                    width: height
                    height: units.gu(2.5)
                    name: "printer-symbolic"
                    SlotsLayout.position: SlotsLayout.First
                }

                ProgressionSlot {}
            }
            onClicked: pageStack.addPageToNextColumn(root, Qt.resolvedUrl("Printer.qml"), {
                printer: model
            })
            Component.onCompleted: console.log("printer", model.name)
        }
    }
}
