/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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
import QtSystemInfo 5.0
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.SystemSettings.StorageAbout 1.0
import Ubuntu.SystemSettings.Bluetooth 1.0
import MeeGo.QOfono 0.2

ItemPage {
    id: root
    objectName: "aboutPage"

    title: i18n.tr("About")
    flickable: scrollWidget
    property var modemsSorted: []

    UbuntuStorageAboutPanel {
        id: backendInfos
    }

    UbuntuBluetoothPanel {
        id: bluetooth
    }

    DeviceInfo {
        id: deviceInfos
    }

    OfonoManager {
        id: manager
        onModemsChanged: root.modemsSorted = modems.slice(0).sort()
    }

    NetworkInfo {
        id: wlinfo
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            Column {
                anchors.left: parent.left
                anchors.right: parent.right
                Label {
                    id: ubuntuLabel
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: contentHeight + units.gu(2)
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: ""
                    fontSize: "x-large"
                }
                Label {
                    id: deviceLabel
                    objectName: "deviceLabel"
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: deviceInfos.manufacturer() ? deviceInfos.manufacturer() + " " + deviceInfos.model() : backendInfos.vendorString
                }
            }

            SettingsListItems.SingleValue {
                id: serialItem
                objectName: "serialItem"
                text: i18n.tr("Serial")
                value: backendInfos.serialNumber
                visible: backendInfos.serialNumber
            }

            SettingsListItems.SingleValue {
                objectName: "imeiItem"
                property string imeiNumber
                imeiNumber: deviceInfos.imei(0)
                text: "IMEI"
                value: modemsSorted.length ? (imeiNumber || i18n.tr("None")) :
                    i18n.tr("None")
                visible: modemsSorted.length == 1
            }

            /* We still need a new ListItem for MultiValue */
            ListItems.MultiValue {
                text: "IMEI"
                objectName: "imeiItems"
                values: {
                    var imeis = [];
                    modemsSorted.forEach(function (path, i) {
                        var imei = deviceInfos.imei(i);
                        imei ? imeis.push(imei) : imeis.push(i18n.tr("None"));
                    });
                    return imeis;
                }
                visible: modemsSorted.length > 1
            }

            SettingsListItems.SingleValue {
                property string address: wlinfo.macAddress(NetworkInfo.WlanMode, 0)
                text: i18n.tr("Wi-Fi address")
                value: address ? address.toUpperCase() : ""
                visible: address
                showDivider: bthwaddr.visible
            }

            SettingsListItems.SingleValue {
                id: bthwaddr
                text: i18n.tr("Bluetooth address")
                value: bluetooth.adapterAddress
                visible: bluetooth.adapterAddress
                showDivider: false
            }

            ListItems.Divider {}

            SettingsListItems.SingleValueProgression {
                id: storageItem
                objectName: "storageItem"
                text: i18n.tr("Storage")
                /* TRANSLATORS: that's the free disk space, indicated in the most appropriate storage unit */
                value: i18n.tr("%1 free").arg(Utilities.formatSize(backendInfos.getFreeSpace("/home")))
                onClicked: pageStack.addPageToNextColumn(root, Qt.resolvedUrl("Storage.qml"))
            }

            SettingsItemTitle {
                id: softwareItem
                objectName: "softwareItem"
                visible: SystemImage.currentBuildNumber
                text: i18n.tr("Software:")
            }

            SettingsListItems.SingleValueProgression {
                visible: softwareItem.visible
                property string versionIdentifier: {
                    var num = SystemImage.currentBuildNumber;
                    var ota = SystemImage.versionTag;
                    num = num ? "r%1".arg(num.toString()) : "";
                    return ota ? ota : num;
                }
                objectName: "osItem"
                text: i18n.tr("OS")
                value: "Ubuntu %1%2"
                    .arg(deviceInfos.version(DeviceInfo.Os))
                    .arg(versionIdentifier ? " (%1)".arg(versionIdentifier) : "")
                onClicked: pageStack.addPageToNextColumn(root, Qt.resolvedUrl("Version.qml"), {
                    version: versionIdentifier
                })
            }

            SettingsListItems.SingleValue {
                objectName: "lastUpdatedItem"
                visible: softwareItem.visible
                text: i18n.tr("Last updated")
                value: SystemImage.lastUpdateDate && !isNaN(SystemImage.lastUpdateDate) ?
                    Qt.formatDate(SystemImage.lastUpdateDate) : i18n.tr("Never")
            }

            SettingsListItems.SingleControl {
                visible: softwareItem.visible

                Button {
                    objectName: "updateButton"
                    text: i18n.tr("Check for updates")
                    width: parent.width - units.gu(4)
                    onClicked: {
                        var upPlugin = pluginManager.getByName("system-update")
                        if (upPlugin) {
                            var updatePage = upPlugin.pageComponent
                            var updatePageItem;
                            if (updatePage) {
                                updatePageItem = pageStack.addPageToNextColumn(root, updatePage, {
                                    plugin: upPlugin, pluginManager: pluginManager
                                });
                                updatePageItem.check(true); // Force a check.
                            } else {
                                console.warn("Failed to get system-update pageComponent")
                            }
                        } else {
                            console.warn("Failed to get system-update plugin instance")
                        }
                    }
                }
                showDivider: false
            }

            SettingsItemTitle {
                objectName: "legalItem"
                text: i18n.tr("Legal:")
            }

            SettingsListItems.StandardProgression {
                objectName: "licenseItem"
                text: i18n.tr("Software licenses")
                onClicked: pageStack.addPageToNextColumn(root, Qt.resolvedUrl("Software.qml"))
            }

            SettingsListItems.StandardProgression {
                property var regulatoryInfo:
                    pluginManager.getByName("regulatory-information")
                text: i18n.tr("Regulatory info")
                visible: regulatoryInfo
                onClicked: pageStack.addPageToNextColumn(root, regulatoryInfo.pageComponent)
            }

            SettingsListItems.SingleValueProgression {
                objectName: "devmodeItem"
                text: i18n.tr("Developer mode")
                visible: !isSnap && backendInfos.developerModeCapable || showAllUI
                onClicked: pageStack.addPageToNextColumn(root, Qt.resolvedUrl("DevMode.qml"))
            }
        }
    }
}
