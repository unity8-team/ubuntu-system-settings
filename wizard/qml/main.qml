/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import GSettings 1.0
import Ubuntu.Components 0.1
import Unity.Application 0.1
import Unity.Notifications 1.0 as NotificationBackend
import "file:///usr/share/unity8/Notifications" as Notifications // FIXME This should become a module or go away

Item {
    id: root
    width: units.gu(40)
    height: units.gu(71)

    Component.onCompleted: {
        Theme.name = "Ubuntu.Components.Themes.SuruGradient"
        i18n.domain = "ubuntu-system-settings"
    }

    // This is just a little screen to immediately go to black once the wizard
    // is done, to give quick feedback to the user.
    Rectangle {
        id: blackCover
        color: "#000000"
        anchors.fill: parent
        z: 1
        visible: false
    }

    MainView {
        anchors.fill: parent
        headerColor: "#57365E"
        backgroundColor: "#A55263"
        footerColor: "#D75669"

        GSettings {
            id: background
            schema.id: "org.gnome.desktop.background"
        }

        Image {
            id: image
            anchors.fill: parent
            source: background.pictureUri
            fillMode: Image.PreserveAspectCrop
            visible: status === Image.Ready
        }

        PageStack {
            id: pageStack

            function next() {
                // If we've opened any extra (non-main) pages, pop them before
                // continuing so back button returns to the previous main page.
                while (pageList.index < pageStack.depth - 1)
                    pop()
                push(pageList.next())
            }

            function prev() {
                pageList.prev() // to update pageList.index
                pop()
            }

            Component.onCompleted: next()
        }
    }

    OSKController {
        anchors.fill: parent
    }

    Notifications.Notifications {
        id: notifications
        model: NotificationBackend.Model
        margin: units.gu(1)
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
        states: [
            State {
                name: "narrow"
                when: parent.width <= units.gu(60)
                AnchorChanges { target: notifications; anchors.left: parent.left }
            },
            State {
                name: "wide"
                when: parent.width > units.gu(60)
                AnchorChanges { target: notifications; anchors.left: undefined }
                PropertyChanges { target: notifications; width: units.gu(38) }
            }
        ]

//        FIXME despite correctly turning blockInput: false when the notification disappears, input is still blocked. Commenting out for now
//        InputFilterArea {
//            anchors { left: parent.left; right: parent.right }
//            height: parent.contentHeight
//            blockInput: height > 0
//        }
    }
}
