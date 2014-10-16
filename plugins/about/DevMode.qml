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
import Ubuntu.SystemSettings.SecurityPrivacy 1.0
import Ubuntu.SystemSettings.StorageAbout 1.0

ItemPage {
    id: devModePage
    objectName: "devModePage"
    title: i18n.tr("Developer Mode")
    flickable: scrollWidget

    UbuntuStorageAboutPanel {
        id: storedInfo
    }

    UbuntuSecurityPrivacyPanel {
        id: securityPrivacy
    }

    onActiveChanged: devModeSwitch.checked = storedInfo.developerMode

    Flickable {
        id: scrollWidget
        anchors.fill: parent

        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > devModePage.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

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
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                        text: i18n.tr("In Developer Mode, anyone can access, change or delete anything on this phone by connecting it to another device.")
                    }
                }
            }

            ListItem.Standard {
                text: i18n.tr("Developer Mode")
                enabled: securityPrivacy.securityType !== UbuntuSecurityPrivacyPanel.Swipe
                control: Switch {
                    id: devModeSwitch
                    checked: storedInfo.developerMode
                    onClicked: storedInfo.developerMode = checked
                }
                showDivider: false
            }

            ListItem.Divider {}

            ListItem.SingleValue {
                height: lockSecurityLabel.height + units.gu(2)
                Label {
                    id: lockSecurityLabel
                    width: parent.width
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    text: i18n.tr("You need a passcode or passphrase set to use Developer Mode.")
                }
            }

            ListItem.SingleValue {
                objectName: "lockSecurityItem"
                text: i18n.tr("Lock security")
                progression: true
                onClicked: pageStack.push(Qt.resolvedUrl("../security-privacy/LockSecurity.qml"))
            }
        }
    }
}

