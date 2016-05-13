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
import Ubuntu.SystemSettings.Notifications 1.0
import SystemSettings 1.0

ItemPage {
    id: appsVibrationsNotifyPage
    objectName: "appsVibrationsNotifyPage"

    property alias model: appsVibrationsNotifyList.model

    title: i18n.tr("Vibration")

    ListView {
        id: appsVibrationsNotifyList
        objectName: "appsVibrationNotifyList"

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        clip: true
        contentHeight: contentItem.childrenRect.height

        header: Column {
            anchors {
                left: parent.left
                right: parent.right
            }

            ListItem {
                ListItemLayout { title.text: i18n.tr("When in Silent mode:") }
            }

            ListItem {
                height: layout.height + (divider.visible ? divider.height : 0)
                SlotsLayout {
                    id: layout

                    mainSlot: Item {
                        height: optionSelector.itemHeight * 2
                        width: parent.width - 2 * (layout.padding.leading + layout.padding.trailing)
                        OptionSelector {
                            id: optionSelector
                            expanded: true
                            model: [i18n.tr("Vibrate as normal"),
                                    i18n.tr("Don't vibrate")]
                        }
                    }
                }
            }
 
            ListItem {
                ListItemLayout { title.text: i18n.tr("All installed apps:") }
            }
        }

        delegate: ListItem {
            ListItemLayout {
                Component.onCompleted: {
                    var iconPath = model.icon.toString()
                    if (iconPath.search("/") == -1) {
                        icon.name = model.icon
                    } else {
                        icon.source = model.icon
                    }
                }

                title.text: model.displayName

                Row {
                    spacing: units.gu(2)

                    SlotsLayout.position: SlotsLayout.Leading;

                    CheckBox {
                        anchors.verticalCenter: icon.verticalCenter
                        checked: model.vibrationsNotify
                    }

                    Icon {
                        id: icon
                        width: units.gu(5)
                    }
                }
            }
        }
    }
}
