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
    id: appsSoundsNotifyPage
    objectName: "appsSoundsNotifyPage"

    property alias model: appsSoundsNotifyList.model
    
    property var uncheckedIndexes: []

    title: i18n.tr("Sound")

    ListView {
        id: appsSoundsNotifyList
        objectName: "appsSoundsNotifyList"

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        clip: true
        contentHeight: contentItem.childrenRect.height

        header: ListItem {
            ListItemLayout { title.text: i18n.tr("Apps that notify with sound:") }
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
                        checked: model.soundsNotify

                        onCheckedChanged: {
                            if (!checked) {
                                uncheckedIndexes.push(index)
                            } else {
                                var i = uncheckedIndexes.indexOf(index)
                                if (i >= 0) {
                                    uncheckedIndexes.splice(i, 1)
                                }
                            }
                        }
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
