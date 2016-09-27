/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Ken VanDine <ken.vandine@canonical.com>
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

import QtQuick 2.0
import Ubuntu.Components 1.3

Item {
    /* this property will come from Mir */
    property int doubleTapSpeed: 1000
    property var message: ""
    property bool lastStatus
    property var button: doubleTapButton

    Timer {
        id: clickTimer
        objectName: "clickTimer"
        triggeredOnStart: false
        repeat: false
        interval: doubleTapSpeed
        onTriggered: {
            message = i18n.tr("Not fast enough")
            lastStatus = false;
            doubleTapButton.text = "😌";
            safetyDelayTimer.start();
        }
    }

    Timer {
        id: safetyDelayTimer
        triggeredOnStart: false
        repeat: false
        interval: 2000
        onTriggered: {
            doubleTapButton.text = "😴";
        }
    }

    AbstractButton {
        id: doubleTapButton
        objectName: "doubleTapButton"
        text: "😴"
        height: parent.height
        width: height
        enabled: !safetyDelayTimer.running
        UbuntuShape {
            anchors.fill: parent
            color: "lightgray"

            Label {
                anchors.centerIn: parent
                text: doubleTapButton.text
            }
        }

        onClicked: {
            if (clickTimer.running) {
                clickTimer.stop();
                safetyDelayTimer.start();
                text = "😀";
                message = i18n.tr("Double-clicked");
                lastStatus = true;
                return;
            } else {
                clickTimer.start()
            }
        }
    }

    Label {
        id: label
        objectName: "label"
        anchors {
            left: doubleTapButton.right
            leftMargin: units.gu(2)
            verticalCenter: parent.verticalCenter
        }
        visible: safetyDelayTimer.running
        text: message
    }
}
