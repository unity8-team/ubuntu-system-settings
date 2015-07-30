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
import SystemSettings 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.1 as ListItem
import Ubuntu.Settings.Menus 0.1 as Menus


Column {
    anchors.left: parent.left
    anchors.right: parent.right
    property bool tappablePad: true
    property bool scrollTwoSupported: true
    property int numberOfButtons: 3
    
    ListItem.Header {
        text: i18n.tr("Mouse")
    }

    SettingsItemTitle {
        text: i18n.tr("Move:")
        showDivider: false
    }

    Slider {
        anchors { 
            left: parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        id: mouseMoveSpeed
        objectName: "mouseMoveSpeed"
        function formatValue(v) { return v.toFixed(2) }
        minimumValue: 0.0
        maximumValue: 1.0
    }

    SettingsItemTitle {
        text: i18n.tr("Scroll:")
        showDivider: false
    }

    Slider {
        anchors { 
            left: parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        id: mouseScrollSpeed
        objectName: "mouseScrollSpeed"
        function formatValue(v) { return v.toFixed(2) }
        minimumValue: 0.0
        maximumValue: 1.0
    }

    SettingsItemTitle {
        text: i18n.tr("Double-click:")
        showDivider: false
    }

    Slider {
        anchors { 
            left: parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        id: mouseClickSpeed
        objectName: "mouseClickSpeed"
        function formatValue(v) { return v.toFixed(2) }
        minimumValue: 0.0
        maximumValue: 1.0
    }

    SettingsItemTitle {
        text: i18n.tr("Test double-click:")
        showDivider: false
    }

    TapArea {
        anchors { 
            left: parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        height: units.gu(5)
    }

    OptionSelector {
        anchors { 
            left: parent.left
            right: parent.right
            topMargin: units.gu(2)
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        text: i18n.tr("Primary button:")
        model: [i18n.tr("Left"),
                i18n.tr("Right")]
    }

    ListItem.Header {
        text: i18n.tr("Touchpad")
    }
                
    SettingsItemTitle {
        text: i18n.tr("Move:")
        showDivider: false
    }

    Slider {
        anchors { 
            left: parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        id: touchMoveSpeed
        objectName: "touchMoveSpeed"
        function formatValue(v) { return v.toFixed(2) }
        minimumValue: 0.0
        maximumValue: 1.0
        
    }

    SettingsItemTitle {
        text: i18n.tr("Scroll:")
        showDivider: false
    }

    Slider {
        anchors { 
            left: parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        id: touchScrollSpeed
        objectName: "touchScrollSpeed"
        function formatValue(v) { return v.toFixed(2) }
        minimumValue: 0.0
        maximumValue: 1.0
    }

    SettingsItemTitle {
        text: i18n.tr("Double-click:")
        showDivider: false
    }

    Slider {
        anchors { 
            left: parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        id: touchClickSpeed
        objectName: "touchClickSpeed"
        function formatValue(v) { return v.toFixed(2) }
        minimumValue: 0.0
        maximumValue: 1.0
    }

    SettingsItemTitle {
        text: i18n.tr("Test double-click:")
        showDivider: false
    }

    TapArea {
        anchors { 
            left: parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        height: units.gu(5)
    }

    OptionSelector {
        visible: numberOfButtons > 1
        anchors { 
            left: parent.left
            right: parent.right
            topMargin: units.gu(2)
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        text: i18n.tr("Primary button:")
        model: [i18n.tr("Left"),
                i18n.tr("Right")]
    }

    ListItem.Standard {
        visible: tappablePad
        text: i18n.tr("Tap to click")
        control: CheckBox {}
        showDivider: false
    }

    ListItem.Standard {
        visible: scrollTwoSupported
        text: i18n.tr("Scroll with two fingers")
        control: CheckBox {}
        showDivider: false
    }

    OptionSelector {
        anchors { 
            left: parent.left
            right: parent.right
            topMargin: units.gu(2)
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        text: i18n.tr("Ignore touchpad when:")
        expanded: true
        multiSelection: true
        model: [i18n.tr("Typing"),
                i18n.tr("A mouse is connected")]
    }
}
