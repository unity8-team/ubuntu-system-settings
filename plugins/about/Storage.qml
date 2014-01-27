/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Sebastien Bacher <sebastien.bacher@canonical.com>
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
import QtSystemInfo 5.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.StorageAbout 1.0

ItemPage {
    id: storagePage

    objectName: "storagePage"
    title: i18n.tr("Storage")
    flickable: scrollWidget

    property bool sortByName: settingsId.storageSortByName
    property var allDrives: {
        var drives = ["/"] // Always consider /
        var paths = [backendInfo.getDevicePath("/")]
        var systemDrives = storageInfo.allLogicalDrives
        for (var i = 0; i < systemDrives.length; i++) {
            var drive = systemDrives[i]
            var type = storageInfo.driveType(drive)
            var path = backendInfo.getDevicePath(drive)
            if ((type === StorageInfo.InternalDrive ||
                type === StorageInfo.RemovableDrive ||
                type === StorageInfo.UnknownDrive) &&
                paths.indexOf(path) == -1 && // Haven't seen this device before
                path.charAt(0) === "/") { // Has a real mount point
                drives.push(drive)
                paths.push(path)
            }
        }
        return drives
    }
    property real diskSpace: {
        var space = 0
        for (var i = 0; i < allDrives.length; i++) {
            space += storageInfo.totalDiskSpace(allDrives[i])
        }
        return space
    }
    property real freediskSpace: {
        var space = 0
        for (var i = 0; i < allDrives.length; i++) {
            space += storageInfo.availableDiskSpace(allDrives[i])
        }
        return space
    }
    property real usedByUbuntu: diskSpace -
                                freediskSpace -
                                backendInfo.homeSize -
                                backendInfo.totalClickSize
    property real otherSize: diskSpace -
                             freediskSpace -
                             usedByUbuntu -
                             backendInfo.moviesSize -
                             backendInfo.picturesSize -
                             backendInfo.audioSize
    property variant spaceColors: [
        UbuntuColors.orange,
        "red",
        "blue",
        "green",
        "yellow",
        UbuntuColors.lightAubergine]
    property variant spaceLabels: [
        i18n.tr("Used by Ubuntu"),
        i18n.tr("Movies"),
        i18n.tr("Audio"),
        i18n.tr("Pictures"),
        i18n.tr("Other files"),
        i18n.tr("Used by apps")]
    property variant spaceValues: [
        usedByUbuntu, // Used by Ubuntu
        backendInfo.moviesSize,
        backendInfo.audioSize,
        backendInfo.picturesSize,
        otherSize, //Other Files
        backendInfo.totalClickSize]
    property variant spaceObjectNames: [
        "usedByUbuntuItem",
        "moviesItem",
        "audioItem",
        "picturesItem",
        "otherFilesItem",
        "usedByAppsItem"]

    GSettings {
        id: settingsId
        schema.id: "com.ubuntu.touch.system-settings"
        onChanged: {
            if (key == "storageSortByName")
                sortByName = value
        }
    }

    UbuntuStorageAboutPanel {
        id: backendInfo
        property bool ready: false
        // All of these events come simultaneously
        onMoviesSizeChanged: ready = true
        Component.onCompleted: populateSizes()
    }

    StorageInfo {
        id: storageInfo
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: columnId.height

        Column {
            id: columnId
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.SingleValue {
                id: diskItem
                objectName: "diskItem"
                text: i18n.tr("Total storage")
                value: backendInfo.formatSize(diskSpace)
                showDivider: false
            }

            StorageBar {
                ready: backendInfo.ready
            }

            StorageItem {
                objectName: "storageItem"
                colorName: "white"
                label: i18n.tr("Free space")
                value: freediskSpace
                ready: backendInfo.ready
            }

            Repeater {
                model: spaceColors

                StorageItem {
                    objectName: spaceObjectNames[index]
                    colorName: modelData
                    label: spaceLabels[index]
                    value: spaceValues[index]
                    ready: backendInfo.ready
                }
            }

            ListItem.ItemSelector {
                id: valueSelect
                objectName: "installedAppsItemSelector"
                model: [i18n.tr("By name"), i18n.tr("By size")]
                selectedIndex:
                    (backendInfo.sortRole === ClickRoles.DisplayNameRole) ?
                        0 :
                        1
                onSelectedIndexChanged: {
                    backendInfo.sortRole =
                            (selectedIndex == 0) ?
                                ClickRoles.DisplayNameRole :
                                ClickRoles.InstalledSizeRole
                }
            }

            ListView {
                objectName: "installedAppsListView"
                anchors.left: parent.left
                anchors.right: parent.right
                height: childrenRect.height
                /* Desactivate the listview flicking, we want to scroll on the column */
                interactive: false
                model: backendInfo.clickList
                delegate: ListItem.SingleValue {
                    objectName: "appItem" + displayName
                    icon: iconPath
                    fallbackIconSource: "image://theme/clear"   // TOFIX: use proper fallback
                    text: displayName
                    value: installedSize ?
                               backendInfo.formatSize(installedSize) :
                               i18n.tr("N/A")
                }
            }
        }
    }
}
