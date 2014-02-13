/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
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

Loader {
    id: pageLoader
    property variant options

    function push(page, opts) {
        pageLoader.sourceComponent = page
        pageLoader.options = opts
    }

    visible: false

    onLoaded: {
        timer.running = false
        pageStack.push(pageLoader, options)
        visible = true
    }

    onStatusChanged: {
        if (status != Loader.Ready) {
            timer.running = true
            visible = false
        }
    }

    Timer {
        id: timer
        interval: 5000
        running: false
        /*
        Display an activityindicator
        onTriggered: console.log("ping")
        */
    }
}
