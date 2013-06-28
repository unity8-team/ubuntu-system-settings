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
    id: eraseEverything
    Dialog {
        id: dialog
        text: i18n.tr("All documents, saved games, settings, and other items will be permanently deleted from this phone.")
        Button {
            text: i18n.tr("Erase & reset everything")
            onClicked: PopupUtils.close(dialog)
        }
        Button {
            text: i18n.tr("Cancel")
            onClicked: PopupUtils.close(dialog)
        }
    }
}
