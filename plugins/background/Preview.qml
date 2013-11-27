/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
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
 * Ken VanDine <ken.vandine@canonical.com>
 *
*/

import QtQuick 2.0
import GSettings 1.0
import Ubuntu.Components 0.1
import SystemSettings 1.0
import Ubuntu.SystemSettings.Background 1.0

ItemPage {
    id: preview
    anchors.fill: parent

    title: i18n.tr("Preview")

    property string uri

    Action {
        id: cancelAction
        text: i18n.tr("Cancel")
        iconName: "back"
        onTriggered: {
                pageStack.pop();
        }
    }

    Action {
        id: setAction
        text: i18n.tr("Set")
        iconName: "import-image"
        onTriggered: {
                pageStack.pop();
        }
    }


    tools: ToolbarItems {
        back: ToolbarButton {
            action: cancelAction
        }
        ToolbarButton {
            action: setAction
        }
    }

    UbuntuBackgroundPanel {
        id: backgroundPanel
    }

    Image {
        id: previewImage
        anchors.fill: parent
        source: uri
        width: parent.width
        height: parent.height
        fillMode: Image.PreserveAspectFit
    }
}
