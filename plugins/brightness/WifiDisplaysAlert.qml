/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * Contact: Ken VanDine <ken.vandine@canonical.com>
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
import Ubuntu.SystemSettings.Brightness 1.0

Dialog {
    id: dialog
    property int error
    property string displayName
    text: {
        if (error === AethercastDisplays.Failed)
            return i18n.tr("This device failed to connect to %1.").arg(displayName)
        else if (error === AethercastDisplays.Unknown)
            return i18n.tr("There was an unknown error connecting to %1.").arg(displayName)
    }

    Button {
        text: i18n.tr("OK")
        onClicked: PopupUtils.close(dialog)
    }
}
