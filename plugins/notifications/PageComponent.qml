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
    flickable: notificationsList

    onActiveChanged: {
        if (active) {
            clickAppsSoundsNotifyModel.updateEnabledEntries()
            clickAppsVibrationsNotifyModel.updateEnabledEntries()
        }
    }

    ClickApplicationsNotifyModel {
        id: clickAppsSoundsNotifyModel
        objectName: "clickAppsSoundsNotifyModel"
        notifyType: ClickApplicationsNotifyModel.SoundsNotify
        sourceModel: ClickApplicationsModel
    }

    ClickApplicationsNotifyModel {
        id: clickAppsVibrationsNotifyModel
        objectName: "clickAppsVibrationsNotifyModel"
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
                    text: i18n.tr("Apps can alert you using sounds, vibrations, notification bubbles and the Notification list.")
                    wrapMode: Text.WordWrap
                    color: theme.palette.normal.backgroundSecondaryText
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
                        objectName: "clickAppsSoundsNotifyLabel"
                        text: clickAppsSoundsNotifyModel.count
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

                    pageStack.push(Qt.resolvedUrl("ClickAppsSoundsNotify.qml"),
                                                  { model: clickAppsSoundsNotifyModel })
                }
            }

            ListItem {
                ListItemLayout {
                    title.text: i18n.tr("Apps that notify with vibration");
                    Label {
                        objectName: "clickAppsVibrationsNotifyLabel"
                        text: clickAppsVibrationsNotifyModel.count
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

                    pageStack.push(Qt.resolvedUrl("ClickAppsVibrationsNotify.qml"),
                                                  { model: clickAppsVibrationsNotifyModel })
                }
            }

            ListItem {
                ListItemLayout {
                    title.text: i18n.tr("Applications:")
                    title.color: theme.palette.normal.backgroundSecondaryText
                }
            }

        }

        delegate: ListItem {
            height: layout.height + (divider.visible ? divider.height : 0)

            onClicked: pageStack.push(Qt.resolvedUrl("ClickAppNotifications.qml"),
                                                     { entry: model,
                                                       entryIndex: index })

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
                        return i18n.tr("No notifications")
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
