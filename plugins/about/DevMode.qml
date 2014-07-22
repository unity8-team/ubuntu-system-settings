/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Oliver Grawert <ogra@canonical.com>
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
 *
 * Author: Oliver Grawert <ogra@ubuntu.com>
 *
 */

import QtQuick 2.0
import Qt.labs.folderlistmodel 1.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.StorageAbout 1.0

ItemPage {
    id: versionPage
    objectName: "versionPage"
    title: i18n.tr("Developer Mode")

    UbuntuStorageAboutPanel {
        id: storedInfo
    }

    Flickable {
        anchors.fill: parent

        Column {
            anchors.fill: parent

            ListItem.SingleValue {
                objectName: "devModeWarningItem"
                height: warningColumn.childrenRect.height + units.gu(2)

                Column {
                    anchors.fill: parent
                    anchors.topMargin: units.gu(1)

                    id: warningColumn
                    spacing: units.gu(2)
                    Icon {
                        id: warnIcon
                        width: parent.width/4
                        height: width
                        name: "security-alert"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Label {
                        id: warnText
                        width: parent.width
                        wrapMode: Text.WordWrap
                        text: i18n.tr("In Developer Mode, anyone can access, change or delete anything on this phone by connecting it to another device")
                    }
                }
            }

            ListItem.Standard {
                text: i18n.tr("Developer Mode")
                control: Switch {
                    id: devModeSwitch
                    checked: storedInfo.developerModeState
                    onClicked: storedInfo.developerModeToggle
                }
            }

            ListItem.Divider {}

            ListItem.SingleValue {
                Label {
                    id: "lockSecurityLabel"
                    anchors.verticalCenter: parent.verticalCenter
                    wrapMode: Text.WordWrap
                    text: i18n.tr("You need a passcode or passphrase set to use Developer Mode:")
                }
            }

            ListItem.SingleValue {
                objectName: "lockSecurityItem"
                text: i18n.tr("Lock Security")
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("../security-privacy/LockSecurity.qml"))
            }
        }
    }
}

