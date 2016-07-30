/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
 *
 * Contact: Evan Dandrea <evan.dandrea@canonical.com>
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
import Ubuntu.SystemSettings.Diagnostics 1.0

ItemPage {
    id: root

    title: i18n.tr("Diagnostics")
    flickable: scrollWidget

    UbuntuDiagnostics {
        id: diagnosticsWidget
        function maybeUpdate() {
            reportCrashesCheck.checked = diagnosticsWidget.reportCrashes
        }
        onReportCrashesChanged: maybeUpdate()
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
        /* Set the direction to workaround
           https://bugreports.qt-project.org/browse/QTBUG-31905 otherwise the UI
           might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            SettingsListItems.SingleValueProgression {
                text: i18n.tr("Privacy policy")
                onClicked: {
                    Qt.openUrlExternally("http://www.ubuntu.com/aboutus/privacypolicy?crashdb")
                }
            }

            SettingsListItems.Standard {
                text: i18n.tr("Report to Canonical:")
            }

            DiagnosticsCheckEntry {
                id: reportCrashesCheck
                checked: diagnosticsWidget.reportCrashes
                onCheckedChanged: {
                    diagnosticsWidget.reportCrashes = checked;
                    /* Confirm the setting stuck and reflect it in the UI. */
                    if (checked != diagnosticsWidget.reportCrashes) {
                        checked = !checked;
                    }

                }
                textEntry: i18n.tr("App crashes and errors")
            }

            SettingsListItems.SingleValueProgression {
                id: previousReports
                property string ident: diagnosticsWidget.systemIdentifier()
                text: i18n.tr("Previous error reports")
                enabled: previousReports.ident != ""
                progressionVisible: enabled
                onClicked: {
                    var base = "https://errors.ubuntu.com/user/"
                    Qt.openUrlExternally(base + ident)
                }
            }

            ListItems.Caption {
                text: i18n.tr("Includes info about what an app was doing when it failed.")
            }
        }
    }
}
