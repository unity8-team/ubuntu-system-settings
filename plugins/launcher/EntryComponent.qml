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
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Settings.Components 0.1
import Ubuntu.SystemSettings.Launcher 1.0

Item {
    id: root

    signal clicked

    height: button.height

    objectName: "entryComponent-" + model.item.baseName

    /* The specification require us to hide Launcher if there's no display
    connected larger than 90 GUs. Since we cannot calculate GUs from c++,
    thus hiding this plugin the conventional way, we do it here instead. */
    visible: {
        for (var i=0; i < LauncherPanelPlugin.screens; i++) {
            if (LauncherPanelPlugin.screenGeometry(i).width > units.gu(90)) {
                return true;
            }
        }
        return false;
    }

    onVisibleChanged: {
        if (!visible && pageStack.currentPage.objectName === "launcherPage") {
            pageStack.pop();
        }
    }

    AbstractButton {
        id: button
        anchors.left: parent.left
        anchors.right: parent.right
        onClicked: root.clicked()

        height: col.height

        Column {
            id: col
            anchors.left: parent.left
            anchors.right: parent.right

            Icon {
                id: icon
                anchors.horizontalCenter: parent.horizontalCenter
                width: height
                height: units.gu(4)
                source: model.icon
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18n.dtr(model.item.translations, model.displayName)
                width: col.width
                horizontalAlignment: Text.AlignHCenter
                fontSize: "small"
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
        }
    }

    UbuntuShape {
        z: -1
        visible: button.pressed
        anchors{
            fill: root
            margins: -units.gu(0.25)
        }
        backgroundColor: UbuntuColors.darkGrey
        opacity: 0.15
    }
}
