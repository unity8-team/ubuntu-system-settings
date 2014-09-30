/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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
import Ubuntu.Components.Popups 0.1

Component {
    id: resetLauncherHome
    Dialog {
        id: dialog
        states: State {
            name: "clicked"
            PropertyChanges {
                target: action
                enabled: false
            }
            PropertyChanges {
                target: cancel
                enabled: false
            }
        }
        text: i18n.tr("The Launcher will be returned to its original contents.")
        objectName: "resetLauncherDialog"
        Button {
            id: action
            text: i18n.tr("Reset Launcher")
            objectName: "resetLauncherAction"
            onClicked: {
                dialog.state = "clicked";
                unitySettings.schema.reset("favorites");
                unitySettings.schema.reset("items");
                root.done();
            }
        }
        Button {
            id: cancel
            text: i18n.tr("Cancel")
            onClicked: PopupUtils.close(dialog)
        }
        Label {
            text: i18n.tr("The phone needs to restart for changes to take effect.")
            width: parent.width
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
