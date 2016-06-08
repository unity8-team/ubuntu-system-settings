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
 * This is a Ubuntu DownloadManager singleton.
 */

pragma Singleton

import QtQuick 2.4
import Ubuntu.DownloadManager 1.2
import Ubuntu.SystemSettings.Update 1.0

DownloadManager {
    id: udm

    function getDownload(id) {
        console.warn('getDownload')
        for (var i=0; i < downloads.length; i++) {
            if (id === downloads[i].downloadId) return downloads[i];
        }
        return null;
    }

    onDownloadCanceled: UpdateManager.udmDownloadEnded(download.downloadId) // (SingleDownload download)
    onDownloadFinished: UpdateManager.udmDownloadEnded(download.downloadId) // (SingleDownload download, QString path)
    onErrorFound: UpdateManager.udmDownloadEnded(download.downloadId) // (SingleDownload download)
}
