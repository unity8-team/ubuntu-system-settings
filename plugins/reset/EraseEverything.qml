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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Component {
    id: eraseEverything
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
        objectName: "factoryResetDialog"
        text: i18n.tr("All documents, saved games, settings, and other items will be permanently deleted from this device.")
        Button {
            id: action
            text: i18n.tr("Erase & Reset Everything")
            objectName: "factoryResetAction"
            onClicked: {
                dialog.state = "clicked";
                resetBackend.factoryReset();
                root.done();
            }
        }
        Button {
            id: cancel
            text: i18n.tr("Cancel")
            onClicked: PopupUtils.close(dialog);
        }
    }
}
