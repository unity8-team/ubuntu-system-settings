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
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.Extras.Printers 0.1

Column {
    SettingsListItems.Standard {
        text: i18n.tr("Status")
        layout.subtitle.text: {
            var state;
            if (printer.state == PrinterEnum.IdleState) {
                state = i18n.tr("Idle");
            } else if (printer.state == PrinterEnum.AbortedState) {
                state = i18n.tr("Aborted");
            } else if (printer.state == PrinterEnum.ActiveState) {
                state = i18n.tr("Active");
            } else if (printer.state == PrinterEnum.ErrorState) {
                state = i18n.tr("Stopped");
            }
            /* TRANSLATORS: %1 is the overall state of the printer, %2 is a more
            detailed message from the printer itself. */
            return i18n.tr("%1 — %2")
                .arg(state)
                .arg(printer.lastMessage ?
                     printer.lastMessage : i18n.tr("No messages"));
        }
    }

    SettingsListItems.Standard {
        text: i18n.tr("Jobs")
        layout.subtitle.text: printer.jobs.count ? printer.jobs.count
                                                 : i18n.tr("None")

        Button {
            text: i18n.tr("Manage Print Jobs")
            // Printer names are by the cups definition, URL friendly.
            onClicked: Qt.openUrlExternally("printing:///queue?printer-name=%1".arg(printer.displayName))
        }
    }


    SettingsListItems.Standard {
        text: i18n.tr("Name")
        layout.subtitle.text: printer.displayName
    }

    SettingsListItems.Standard {
        text: i18n.tr("Description")
        layout.subtitle.text: printer.description ? printer.description : ""
        visible: typeof printer.description !== "undefined"
    }

    SettingsListItems.Standard {
        text: i18n.tr("Location")
        layout.subtitle.text: printer.location ? printer.location : ""
        visible: typeof printer.location !== "undefined"
    }

    SettingsListItems.Standard {
        text: i18n.tr("Address")
        layout.subtitle.text: printer.deviceUri
        visible: printer.deviceUri
    }

    SettingsListItems.Standard {
        text: i18n.tr("Model")
        layout.subtitle.text: printer.isRaw ? i18n.tr("Local Raw Printer") : printer.make

        Button {
            text: i18n.tr("Print test page")
            onClicked: Printers.printTestPage(printer.name)
        }
    }
}
