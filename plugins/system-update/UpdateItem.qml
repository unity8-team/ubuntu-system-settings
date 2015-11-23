/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
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
//import Ubuntu.SystemSettings.Update 1.0
import Ubuntu.Components 1.3

ListItem {
    id: updateItem
    property alias actionButton: buttonAppUpdate
    property alias progressBar: progress
    property variant updateData
    property variant tracker
    property bool installing: !updateData.systemUpdate && (updateData.updateReady || (progressBar.value === progressBar.maximumValue))
    property bool installed: false
    property bool retry: false
    height: layout.height + divider.height

    SlotsLayout {
        id: layout
        padding {
            top: units.gu(1)
            bottom: units.gu(1)
        }
        //height: visible ? textArea.height + units.gu(2) : 0
        visible: opacity > 0
        opacity: installed ? 0 : 1
        Behavior on opacity { PropertyAnimation { duration: UbuntuAnimation.SleepyDuration } }

        UbuntuShape {
            SlotsLayout.position: SlotsLayout.Leading
            width: units.gu(4)
            height: width
            source: Image {
                anchors.fill: parent
                source: Qt.resolvedUrl(updateData.iconUrl)
            }
            aspect: updateData.systemUpdate ? UbuntuShape.Flat : UbuntuShape.Inset 
        }

        

        mainSlot: Column {
            id: textArea
            objectName: "textArea"
            SlotsLayout.padding.top: 0
            spacing: units.gu(0.5)
            height: childrenRect.height


            Item {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: childrenRect.height
                Button {
                    id: buttonAppUpdate
                    objectName: "buttonAppUpdate"
                    anchors {
                        top: parent.top
                        right: parent.right
                    }
                    height: labelTitle.height + units.gu(1)
                    enabled: !installing
                    text: {
                        if (retry)
                            return i18n.tr("Retry");
                        if (updateData.systemUpdate) {
                            if (updateData.updateReady) {
                                return i18n.tr("Update");
                            } else if (!updateData.updateState && !updateData.selected) {
                                return i18n.tr("Download");
                            }
                        }
                        if (updateData.updateState) {
                            return i18n.tr("Pause");
                        } else if (updateData.selected) {
                            return i18n.tr("Resume");
                        }
                        return i18n.tr("Update");
                    }

                    onClicked: {
                        if (retry) {
                            retry = false;
                            return UpdateManager.retryDownload(updateData.packageName);
                        }
                        if (updateData.updateState)
                            return pause();
                        if (!updateData.updateState && updateData.selected)
                            return resume();
                        if (!updateData.updateState && !updateData.selected && !updateData.updateReady)
                            return start();
                        if (updateData.updateReady)
                            PopupUtils.open(dialogInstallComponent);
                    }
                }
            
                Label {
                    id: labelTitle
                    objectName: "labelTitle"
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                    text: updateData.title
                    font.bold: true
                    elide: Text.ElideMiddle
                }
            }
            
            Item {
                id: labelUpdateStatus
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: childrenRect.height
                visible: opacity > 0
                opacity: (updateData.updateState && updateData.selected && !updateData.updateReady) || (installing || installed) ? 1 : 0
                Behavior on opacity { PropertyAnimation { duration: UbuntuAnimation.SleepyDuration } }
                Label {
                    objectName: "labelUpdateStatus"
                    anchors.left: parent.left
                    anchors.right: updateStatusLabel.left
                    elide: Text.ElideMiddle
                    fontSize: "small"
                    text: {
                        if (retry)
                            return updateData.error;
                        if (installing)
                            return i18n.tr("Installing");
                        if (installed)
                            return i18n.tr("Installed");
                        return i18n.tr("Downloading");
                    }
                }
                Label {
                    id: updateStatusLabel
                    anchors.right: parent.right
                    visible: !labelSize.visible && !installing && !installed
                    fontSize: "small"
                    text: {
                        if (!labelUpdateStatus.visible)
                            return Utilities.formatSize(updateData.binaryFilesize);

                        return i18n.tr("%1 of %2").arg(
                            Utilities.formatSize(updateData.binaryFilesize * (progress.value * 0.01))).arg(
                            Utilities.formatSize(updateData.binaryFilesize)
                        );
                    }
                }
            }

            ProgressBar {
                id: progress
                objectName: "progress"
                height: units.gu(2)
                anchors {
                    left: parent.left
                    right: parent.right
                }
                visible: opacity > 0
                opacity: updateData.selected && !updateData.updateReady && !installed ? 1 : 0
                value: updateData.systemUpdate ? updateData.downloadProgress : tracker.progress
                minimumValue: 0
                maximumValue: 100

                Behavior on opacity { PropertyAnimation { duration: UbuntuAnimation.SleepyDuration } }
            }

            Item {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: childrenRect.height
                Label {
                    id: labelVersion
                    objectName: "labelVersion"
                    anchors.left: parent.left
                    text: updateData.remoteVersion ? i18n.tr("Version: ") + updateData.remoteVersion : ""
                    elide: Text.ElideRight
                    fontSize: "small"
                }

                Label {
                    id: labelSize
                    objectName: "labelSize"
                    anchors.right: parent.right
                    text: Utilities.formatSize(updateData.binaryFilesize)
                    fontSize: "small"
                    visible: !labelUpdateStatus.visible && !installing && !installed
                }
            }
        }
    }
}
