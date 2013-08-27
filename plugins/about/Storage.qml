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
import QtQuick.XmlListModel 2.0
import QtSystemInfo 5.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.StorageAbout 1.0

ItemPage {
    id: storagePage

    title: i18n.tr("Storage")
    flickable: scrollWidget

    property bool sortByName: settingsId.storageSortByName
    property real diskSpace: storageInfo.totalDiskSpace('/')
    property variant spaceColors: [UbuntuColors.orange, "red", "blue", "green", "yellow", UbuntuColors.lightAubergine]
    property variant spaceLabels: [i18n.tr("Used by Ubuntu"), i18n.tr("Movies"), i18n.tr("Audio"),
                                   i18n.tr("Pictures"), i18n.tr("Other files"), i18n.tr("Used by apps")]
    property variant spaceValues: [19.6, 6.2, 9.2, 1.5, 4.6, 16.3] // TODO: replace by real values

    /* TOFIX: replace by real datas */
    XmlListModel {
        id: xmlModel
        source: "fakepkgslist.xml"
        query: "/list/binary"

        XmlRole { name: "binaryName"; query: "name/string()" }
        XmlRole { name: "iconName"; query: "icon/string()" }
        XmlRole { name: "installedSize"; query: "installed/string()" }

        onStatusChanged: if (status === XmlListModel.Ready) { createSortedLists(); }
    }

    ListModel {
        id: sortedNamesModel
    }

    ListModel {
        id: sortedInstallModel
    }

    GSettings {
        id: settingsId
        schema.id: "com.ubuntu.touch.system-settings"
        onChanged: {
            if (key == "storageSortByName")
                sortByName = value
        }
    }

    ListModel { id: clicksList }

    UbuntuStorageAboutPanel {
        id: backendInfos
        Component.onCompleted: {
            var clickData = getClickList()
            var clickJson = JSON.parse(clickData)
            for (var val in clickJson) {
                clicksList.append({"binaryName": clickJson[val].title})
            }
        }
    }

    function createSortedLists() {
        var n;
        var namesDict = {};
        var nameKeys = [];
        var installDict = {};
        var installKeys = [];

        for (n=0; n < xmlModel.count; n++) {
            namesDict[xmlModel.get(n).binaryName] = [xmlModel.get(n).iconName, xmlModel.get(n).installedSize];
            installDict[xmlModel.get(n).installedSize] = [xmlModel.get(n).iconName, xmlModel.get(n).binaryName];
        }

        nameKeys = Object.keys(namesDict).sort();
        installKeys = Object.keys(installDict).sort(function(a,b){return b-a});

        for (n=0; n < nameKeys.length; n++) {
            sortedNamesModel.append({"binaryName":nameKeys[n],"iconName":namesDict[nameKeys[n]][0],"installedSize":namesDict[nameKeys[n]][1]})
            sortedInstallModel.append({"binaryName":installDict[installKeys[n]][1],"iconName":installDict[installKeys[n]][0],"installedSize":installKeys[n]})
        }
    }

    /* Return used space in a formatted way */
    function getFormattedSpace(space) {
        if (space < 1000)
            return space;
        if (space / 1000 < 1000)
            return Math.round(space / 1000) + " " + i18n.tr("kB");
        else if (space/1000/1000 < 1000)
            return Math.round((space / 1000 / 1000) * 10) / 10 + " " + i18n.tr("MB");
        else if (space/1000/1000/1000 < 1000)
            return Math.round((space / 1000 / 1000 / 1000) * 10) / 10 + " " + i18n.tr("GB");
        return "";
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
                text: i18n.tr("Total storage")
                value: storagePage.getFormattedSpace(diskSpace);
                showDivider: false
            }

            StorageBar {}

            StorageItem {
                colorName: "white"
                label: i18n.tr("Free space")
                value: getFormattedSpace(storageInfo.availableDiskSpace('/'))
            }

            Repeater {
                model: spaceColors

                StorageItem {
                    colorName: modelData
                    label: spaceLabels[index]
                    value: getFormattedSpace(spaceValues[index]*1000000000) // TODO: replace by real values
                }
            }

            ListItem.ValueSelector {
                id: valueSelect
                values: [i18n.tr("By name"), i18n.tr("By size")]
                selectedIndex: sortByName ? 0 : 1
                onSelectedIndexChanged:
                    settingsId.storageSortByName = (valueSelect.selectedIndex == 0) ? true : false
            }

            ListView {
                anchors.left: parent.left
                anchors.right: parent.right
                height: childrenRect.height
                /* Desactivate the listview flicking, we want to scroll on the column */
                interactive: false
                model: (valueSelect.selectedIndex === 0) ? sortedNamesModel : sortedInstallModel
                delegate: ListItem.SingleValue {
                    icon: "image://gicon/" + iconName
                    fallbackIconSource: "image://gicon/clear"   // TOFIX: use proper fallback
                    text: binaryName
                    value: storagePage.getFormattedSpace(installedSize)
                }
            }
        }
    }
}
