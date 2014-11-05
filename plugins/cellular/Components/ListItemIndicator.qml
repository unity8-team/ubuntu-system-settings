/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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
 *
 * This indicator appears in what looks like a OptionSelectorDelegate.
 *
 */
import QtQuick 2.0
import Ubuntu.Components 1.1

Rectangle {
    property alias running: act.running
    property alias text: lbl.text
    anchors {
        margins: units.gu(2)
        left: parent.left
        right: parent.right
    }
    opacity: act.running ? 1 : 0
    color: Theme.palette.normal.background

    ActivityIndicator {
        id: act
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
    }

    Label {
        id: lbl
        anchors {
            left: act.right
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            leftMargin: units.gu(1)
        }
        verticalAlignment: Text.AlignVCenter
    }

    Behavior on opacity {
        NumberAnimation {
            duration: UbuntuAnimation.SnapDuration
        }
    }

    Behavior on height {
        NumberAnimation {
            duration: UbuntuAnimation.SnapDuration
        }
    }
}
