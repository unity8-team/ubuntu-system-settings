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

import QtQuick 2.4
import Ubuntu.Components 1.3

Page {
    id: root

    property alias title: pageHeader.title
    property alias flickable: pageHeader.flickable

    header: PageHeader {
        id: pageHeader
        title: i18n.dtr(plugin.translations, plugin.displayName)
    }

    property variant plugin
    property variant pluginManager
    property variant pluginOptions

    signal pushedOntoStack()

    Connections {
        target: root
        onVisibleChanged: {
            if (visible) {
                pushedOntoStack();
                target = null;
            }
        }
    }
}
