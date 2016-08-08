/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3

Item {
    property string version
    property bool expanded: false

    signal clicked()

    height: label.implicitHeight

    Label {
        id: label
        objectName: "updateVersionLabel"

        anchors {
            left: parent.left
            top: parent.top
        }

        verticalAlignment: Text.AlignVCenter
        text: i18n.tr("Version %1").arg(version)
        fontSize: "small"
        elide: Text.ElideMiddle

    }

    Icon {
        id: icon
        visible: parent.enabled
        name: "next"
        anchors {
            left: label.right
            leftMargin: units.gu(0.5)
            verticalCenter: label.verticalCenter
        }

        rotation: parent.expanded ? 90 : 0
        width: units.gu(1.5)
        height: width

        Behavior on rotation {
            animation: UbuntuNumberAnimation {}
        }
    }

    MouseArea {
        anchors {
            fill: label
            margins: units.gu(-3) // grow hitbox
        }
        onClicked: {
            if (!label.enabled) return;
            parent.clicked()
        }
    }
}
