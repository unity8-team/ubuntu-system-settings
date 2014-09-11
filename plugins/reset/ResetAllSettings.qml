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
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0

Component {
    id: resetAllSettings
    Dialog {
        id: dialog
        text: i18n.tr("The contents and layout of the launcher, and the filters in the home screen will be returned to their original settings.")
        Button {
            text: i18n.tr("Reset all system settings")
            onClicked: {
                pluginManager.resetPlugins()
                PopupUtils.close(dialog)
            }
        }
        Button {
            text: i18n.tr("Cancel")
            onClicked: PopupUtils.close(dialog)
        }
    }
}
