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

import GSettings 1.0
import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Popups 1.0
import Ubuntu.SystemSettings.Reset 1.0

ItemPage {
    id: root

    title: i18n.tr("Reset phone")
    objectName: "resetPage"
    flickable: scrollWidget

    // workaround for #1231729
    // delay destroying popup until pageStack has been popped
    property var popup
    function done () {
        popup.opacity = 0;
        pageStack.pop();
        popup.destroy(1000);
    }

    Loader {
        id: buttonActions
        asynchronous: false
    }

    UbuntuResetPanel {
        id: resetBackend
    }

    GSettings {
        id: unitySettings
        schema.id: "com.canonical.Unity.Launcher"
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
                    objectName: "resetLauncher"
                    text: i18n.tr("Reset Launcher")
                    width: parent.width - units.gu(4)
                    onClicked: {
                        buttonActions.source = "ResetLauncherHome.qml";
                        root.popup = PopupUtils.open(buttonActions.item);
                    }
                }
                showDivider: false
            }

            ListItem.SingleControl {
                visible: false // enabled when backend is ready/useful
                control: Button {
                    id: resetAllSettingsButton
                    text: i18n.tr("Reset all system settings…")
                    width: parent.width - units.gu(4)
                    onClicked: {
                        buttonActions.source = "ResetAllSettings.qml";
                        root.popup = PopupUtils.open(buttonActions.item);
                    }
                }
                showDivider: false
            }

            ListItem.SingleControl {
                control: Button {
                    id: eraseEverythingButton
                    objectName: "factoryReset"
                    text: i18n.tr("Erase & Reset Everything…")
                    width: parent.width - units.gu(4)
                    onClicked: {
                        buttonActions.source = "EraseEverything.qml";
                        root.popup = PopupUtils.open(buttonActions.item);
                    }
                }
                showDivider: false
            }
        }
    }
}
