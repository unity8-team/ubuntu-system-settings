/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2014 Canonical Ltd.
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
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.SystemSettings.Notifications 1.0
import SystemSettings 1.0

ItemPage {
    id: root
    objectName: "systemNotificationsPage"

    title: i18n.tr("Notifications")

    ClickApplicationsNotifyModel {
        id: clickAppsSoundsNotifyModel
        notifyType: ClickApplicationsNotifyModel.SoundsNotify
        sourceModel: ClickApplicationsModel
    }

    ClickApplicationsNotifyModel {
        id: clickAppsVibrationsNotifyModel
        notifyType: ClickApplicationsNotifyModel.VibrationsNotify
        sourceModel: ClickApplicationsModel
    }

    ListView {
        id: notificationsList
        objectName: "notificationsList"
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        model: ClickApplicationsModel
        clip: true

        header: Column {
            anchors {
                left: parent.left
                right: parent.right
            }
 
            ListItems.Base {
                height: labelSubtitle.height + units.gu(2)
                Label {
                    id: labelSubtitle
                    text: i18n.tr("Selected apps can alert you using notification bubbles, sounds, vibrations, and the Notifications list.")
                    wrapMode: Text.WordWrap
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                        topMargin: units.gu(1)
                    }
                }

                highlightWhenPressed: false
            }

            ListItem {
                ListItemLayout {
                    title.text: i18n.tr("Apps that notify with sound");
                    Label {
                        text: clickAppsSoundsNotifyModel.count
                        fontSize: "large"
                        SlotsLayout.position: SlotsLayout.Trailing;
                    }
                    Icon {
                        name: "next"
                        SlotsLayout.position: SlotsLayout.Trailing;
                        width: units.gu(2)
                    }
                }

                onClicked: {
                    if (clickAppsSoundsNotifyModel.count <= 0) {
                        return
                    }

                    var page = pageStack.push(Qt.resolvedUrl("ClickAppsSoundsNotify.qml"), { model: clickAppsSoundsNotifyModel })
                    page.Component.onDestruction.connect( function() {
                        var indexes = page.uncheckedIndexes
                        indexes.sort()
                        for (var i = indexes.length - 1; i >= 0; i--) {
                           clickAppsSoundsNotifyModel.disableNotify(indexes[i])
                        }
                    })
                }
            }

            ListItem {
                ListItemLayout {
                    title.text: i18n.tr("Apps that notify with vibration");
                    Label {
                        text: clickAppsVibrationsNotifyModel.count
                        fontSize: "large"
                        SlotsLayout.position: SlotsLayout.Trailing;
                    }
                    Icon {
                        name: "next"
                        SlotsLayout.position: SlotsLayout.Trailing;
                        width: units.gu(2)
                    }
                }

                onClicked: {
                    if (clickAppsVibrationsNotifyModel.count <= 0) {
                        return
                    }

                    var page = pageStack.push(Qt.resolvedUrl("ClickAppsVibrationsNotify.qml"), { model: clickAppsVibrationsNotifyModel })
                    page.Component.onDestruction.connect( function() {
                        var indexes = page.uncheckedIndexes
                        indexes.sort()
                        for (var i = indexes.length - 1; i >= 0; i--) {
                           clickAppsVibrationsNotifyModel.disableNotify(indexes[i])
                        } 
                    })
                }
            }

            ListItem {
                ListItemLayout { title.text: i18n.tr("All installed apps:") }
            }

        }

        delegate: ListItem {
            height: layout.height + (divider.visible ? divider.height : 0)

            onClicked: {
                var page = pageStack.push(Qt.resolvedUrl("ClickAppNotifications.qml"),
                                                         { title: model.displayName,
                                                           enableNotifications: model.enableNotifications,
                                                           soundsNotify: model.soundsNotify,
                                                           vibrationsNotify: model.vibrationsNotify,
                                                           bubblesNotify: model.bubblesNotify,
                                                           listNotify: model.listNotify })

                page.Component.onDestruction.connect( function() {
                    page.disableNotificationsWhenAllUnchecked()
                    
                    if (!ClickApplicationsModel) {
                        return
                    }

                    ClickApplicationsModel.setNotifyEnabled(ClickApplicationsModel.EnableNotifications, index, page.enableNotifications)
                    ClickApplicationsModel.setNotifyEnabled(ClickApplicationsModel.SoundsNotify, index, page.soundsNotify)
                    ClickApplicationsModel.setNotifyEnabled(ClickApplicationsModel.VibrationsNotify, index, page.vibrationsNotify)
                    ClickApplicationsModel.setNotifyEnabled(ClickApplicationsModel.BubblesNotify, index, page.bubblesNotify)
                    ClickApplicationsModel.setNotifyEnabled(ClickApplicationsModel.ListNotify, index, page.listNotify)
                })
            }

            ListItemLayout {
                id: layout

                Component.onCompleted: {
                    var iconPath = model.icon.toString()
                    if (iconPath.search("/") == -1) {
                        icon.name = model.icon
                    } else {
                        icon.source = model.icon
                    }
                }

                title.text: model.displayName
                subtitle.text: {
                    if (!model.enableNotifications) {
                        return i18n.tr("Disabled")
                    }

                    var arr = []
                    if (model.soundsNotify) {
                        arr.push(i18n.tr("Sounds"))
                    }
                    if (model.vibrationsNotify) {
                        arr.push(i18n.tr("Vibrations"))
                    }
                    if (model.bubblesNotify) {
                        arr.push(i18n.tr("Bubbles"))
                    }
                    if (model.listNotify) {
                        arr.push(i18n.tr("Notification List"))
                    }

                    return arr.join(", ")
                }
                Icon {
                    id: icon
                    SlotsLayout.position: SlotsLayout.Leading;
                    width: units.gu(5)
                }
                Icon {
                    name: "next"
                    SlotsLayout.position: SlotsLayout.Trailing;
                    width: units.gu(2)
                }
            }
        }
    }
}
