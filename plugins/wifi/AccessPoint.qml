/*
 * Copyright 2013-2016 Canonical Ltd.
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
 *
 * Authors:
 *      Renato Araujo Oliveira Filho <renato@canonical.com>
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

FramedMenuItem {
    id: accessPoint

    property bool checked: false
    property bool secure: false
    property bool adHoc: false
    property int signalStrength: 0
    iconName: {
        var imageName = "nm-signal-100"

        if (adHoc) {
            imageName = "nm-adhoc";
        } else if (signalStrength == 0) {
            imageName = "nm-signal-00";
        } else if (signalStrength <= 25) {
            imageName = "nm-signal-25";
        } else if (signalStrength <= 50) {
            imageName = "nm-signal-50";
        } else if (signalStrength <= 75) {
            imageName = "nm-signal-75";
        }

        if (secure) {
            imageName += "-secure";
        }
        return imageName;
    }
    layout.subtitle.text: checked ? i18n.tr("Connected") : ""

    signal activate()

    progressionVisible: checked
    onClicked: {
        if (checked) {
            pageStack.addPageToNextColumn(wifibase,
                Qt.resolvedUrl("NetworkDetailsBrief.qml"),
                {networkName : text, accessPoint: accessPoint})
        } else {
            accessPoint.activate();
        }
    }
}
