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

MainView {
    width: units.gu(40)
    height: units.gu(71)

    headerColor: "#57365E"
    backgroundColor: "#A55263"
    footerColor: "#D75669"

    Component.onCompleted: Theme.name = "Ubuntu.Components.Themes.SuruGradient"

    OSKController {
        anchors.fill: parent
    }

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
        Component.onCompleted: push(Qt.resolvedUrl("Pages/WelcomePage.qml"))
    }
}
