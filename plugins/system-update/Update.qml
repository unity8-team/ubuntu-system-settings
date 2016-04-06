/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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
 * An update component, visually representing a non-installed update.
 * This component is meant to be software agnostic, meaning it doesn't
 * care whether or not it is a Click/Snappy/System update.
 */

import QtQuick 2.4

Item {

    property string name
    property int status // This is a DownloadManager::UpdateStatus enum
    property string version
    property string size
    property string changelog
    property int progress // From 0 to 100

    signal retry()
    signal download()
    signal install()
    signal pause()

    function setError(errorString) {

    }
}
