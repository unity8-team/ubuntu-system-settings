/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-14 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.Brightness 1.0
import Ubuntu.Settings.Menus 0.1 as Menus

Column {
    visible: showAllUI
    objectName: "displayConfiguration_" + displayName
    anchors {
        left: parent.left
        right: parent.right
    }

    Item {
        width: units.gu(1)
        height: units.gu(1)
    }

    Label {
        text: displayName

        anchors {
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }

        Switch {
            objectName: "enabledSwitch"
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            checked: model.enabled
            onCheckedChanged: model.enabled = checked
        }
    }

    SettingsItemTitle {
        text: i18n.tr("Rotation:")
    }

    OptionSelector {
        id: rotationSelector
        objectName: "rotationSelector"
        property bool _expanded: false
        anchors {
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }
        containerHeight: itemHeight * 4
        model: [
            /* TRANSLATORS: None means no rotation, or
            0 degrees. */
            i18n.tr("None"),
            i18n.tr("90° clockwise"),
            i18n.tr("180°"),
            i18n.tr("270°")
        ]
        onDelegateClicked: {
            expanded = !currentlyExpanded;
            orientation = index;
        }
        selectedIndex: orientation
    }

    SettingsItemTitle {
        objectName: "resolutionLabel"
        text: availableModes.length > 1 ?
            i18n.tr("Resolution:") :
            /* TRANSLATORS: %1 is a display resolution, e.g.
            1200x720x24. Unknown refers to an unknown
            resolution (in case of an error). */
            i18n.tr("Resolution: %1").arg(availableModes[mode] || i18n.tr("Unknown"))
    }

    OptionSelector {
        id: resolutionSelector
        objectName: "resolutionSelector"
        property bool _expanded: false
        anchors {
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }
        visible: availableModes.length > 1
        containerHeight: itemHeight * availableModes.length
        model: availableModes
        onDelegateClicked: {
            expanded = !currentlyExpanded;
            mode = index;
        }
        delegate: OptionSelectorDelegate {
            text: modelData
        }
        selectedIndex: mode
    }

    SettingsItemTitle {
        text: i18n.tr("Scale screen elements:")
    }

    Menus.SliderMenu {
        id: scaleSlider
        objectName: "scaleSlider"
        minimumValue: 0.5
        maximumValue: 3.0
        value: model.scale
        minIcon: "image://theme/grip-large"
        maxIcon: "image://theme/view-grid-symbolic"
        function formatValue(v) { return v.toFixed(2) }
        live: true
        onValueChanged: model.scale = value
    }
}
