/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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
import Ubuntu.Components 0.1
import SystemSettings 1.0
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1

ItemPage {
    id: root

    title: i18n.tr("Reset phone")
    flickable: scrollWidget

    Loader {
        id: buttonActions
        asynchronous: false
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.SingleControl {
                control: Button {
                    id: resetLauncherHomeButton
                    text: i18n.tr("Reset launcher & home screen…")
                    width: parent.width - units.gu(4)
                    onClicked: {
                        buttonActions.source = "ResetLauncherHome.qml"
                        PopupUtils.open(buttonActions.item)
                    }
                }
                showDivider: false
            }

            ListItem.SingleControl {
                control: Button {
                    id: resetAllSettingsButton
                    text: i18n.tr("Reset all system settings…")
                    width: parent.width - units.gu(4)
                    onClicked: {
                        buttonActions.source = "ResetAllSettings.qml"
                        PopupUtils.open(buttonActions.item)
                    }
                }
                showDivider: false
            }

            ListItem.SingleControl {
                control: Button {
                    id: eraseEverythingButton
                    text: i18n.tr("Erase & reset everything…")
                    width: parent.width - units.gu(4)
                    onClicked: {
                        buttonActions.source = "EraseEverything.qml"
                        PopupUtils.open(buttonActions.item)
                    }
                }
                showDivider: false
            }
        }
    }
}
