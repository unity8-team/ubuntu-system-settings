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
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.2 as ListItem
import Ubuntu.Settings.Menus 0.1 as Menus
import Ubuntu.SystemSettings.Mouse 1.0
import Ubuntu.Settings.Components 0.1 as USC

Column {
    anchors.left: parent.left
    anchors.right: parent.right
    property bool tappablePad: true
    property bool scrollTwoSupported: true
    property int numberOfButtons: 3
    
    Mouse { id: mouse }
    TouchPad { id: touchPad }

    ListItem.Header {
        text: i18n.tr("Mouse")
    }

    SettingsItemTitle {
        text: i18n.tr("Move:")
        showDivider: false
    }

        Menus.SliderMenu {
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
        value: mouse.cursorSpeed
        live: true
        property real serverValue: enabled ? mouse.cursorSpeed : 0.0
        USC.ServerPropertySynchroniser {
            userTarget: mouseMoveSpeed
            userProperty: "value"
            serverTarget: mouseMoveSpeed
            serverProperty: "serverValue"
            maximumWaitBufferInterval: 16
            
            onSyncTriggered: mouse.cursorSpeed = value
        }
    }

    SettingsItemTitle {
        text: i18n.tr("Scroll:")
        showDivider: false
    }

    Menus.SliderMenu {
        anchors { 
            left: parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        id: mouseScrollSpeed
        objectName: "mouseMoveSpeed"
        function formatValue(v) { return v.toFixed(2) }
        minimumValue: 0.0
        maximumValue: 1.0
        value: mouse.scrollSpeed
        live: true
        property real serverValue: enabled ? mouse.scrollSpeed : 0.0
        USC.ServerPropertySynchroniser {
            userTarget: mouseScrollSpeed
            userProperty: "value"
            serverTarget: mouseScrollSpeed
            serverProperty: "serverValue"
            maximumWaitBufferInterval: 16
            
            onSyncTriggered: mouse.scrollSpeed = value
        }
    }
    SettingsItemTitle {
        text: i18n.tr("Double-click:")
        showDivider: false
    }

    Menus.SliderMenu {
        anchors { 
            left: parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        id: mouseClickSpeed
        objectName: "mouseClickSpeed"
        function formatValue(v) { return v.toFixed(2) }
        minimumValue: 100
        maximumValue: 1000
        value: mouse.clickSpeed
        live: true
        property real serverValue: enabled ? mouse.clickSpeed : 0.0
        USC.ServerPropertySynchroniser {
            userTarget: mouseClickSpeed
            userProperty: "value"
            serverTarget: mouseClickSpeed
            serverProperty: "serverValue"
            maximumWaitBufferInterval: 16
            
            onSyncTriggered: mouse.clickSpeed = value
        }
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
        doubleTapSpeed: mouse.clickSpeed
    }

    ListItem.ItemSelector {
        anchors {
            left: parent.left
            right: parent.right
        }
        activeFocusOnPress: false
        text: i18n.tr("Primary button:")
        model: [i18n.tr("Left"),
                i18n.tr("Right")]
        onDelegateClicked: {
            for (var item in model) {
                console.warn(item);
            }
        }
    }

    ListItem.Header {
        text: i18n.tr("Touchpad")
    }
                
    SettingsItemTitle {
        text: i18n.tr("Move:")
        showDivider: false
    }

    Menus.SliderMenu {
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
        value: touchPad.cursorSpeed
        live: true
        property real serverValue: enabled ? touchPad.cursorSpeed : 0.0
        USC.ServerPropertySynchroniser {
            userTarget: touchMoveSpeed
            userProperty: "value"
            serverTarget: touchMoveSpeed
            serverProperty: "serverValue"
            maximumWaitBufferInterval: 16
            
            onSyncTriggered: touchPad.cursorSpeed = value
        }
    }

    SettingsItemTitle {
        text: i18n.tr("Scroll:")
        showDivider: false
    }

    Menus.SliderMenu {
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
        value: touchPad.scrollSpeed
        live: true
        property real serverValue: enabled ? touchPad.scrollSpeed : 0.0
        USC.ServerPropertySynchroniser {
            userTarget: touchScrollSpeed
            userProperty: "value"
            serverTarget: touchScrollSpeed
            serverProperty: "serverValue"
            maximumWaitBufferInterval: 16
            
            onSyncTriggered: touchPad.scrollSpeed = value
        }
    }

    SettingsItemTitle {
        text: i18n.tr("Double-click:")
        showDivider: false
    }

    Menus.SliderMenu {
        anchors { 
            left: parent.left
            right: parent.right
            leftMargin: units.gu(2)
            rightMargin: units.gu(2)
        }
        id: touchClickSpeed
        objectName: "touchClickSpeed"
        function formatValue(v) { return v.toFixed(2) }
        minimumValue: 100
        maximumValue: 1000
        value: touchPad.clickSpeed
        live: true
        property real serverValue: enabled ? touchPad.clickSpeed : 0.0
        USC.ServerPropertySynchroniser {
            userTarget: touchClickSpeed
            userProperty: "value"
            serverTarget: touchClickSpeed
            serverProperty: "serverValue"
            maximumWaitBufferInterval: 16
            
            onSyncTriggered: touchPad.clickSpeed = value
        }
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
        doubleTapSpeed: touchPad.clickSpeed
    }

    ListItem.ItemSelector {
        anchors {
            left: parent.left
            right: parent.right
        }
        activeFocusOnPress: false
        text: i18n.tr("Primary button:")
        model: [i18n.tr("Left"),
                i18n.tr("Right")]
        onDelegateClicked: {
            for (var item in model) {
                console.warn(item);
            }
        }
    }

    ListItem.Standard {
        id: tapToClick
        text: i18n.tr("Tap to click")
        control: CheckBox {
            property bool serverChecked: touchPad.tapToClick
            onServerCheckedChanged: checked = serverChecked
            Component.onCompleted: checked = serverChecked
            onTriggered: touchPad.tapToClick = checked
        }
        showDivider: false
    }

    ListItem.Standard {
        visible: scrollTwoSupported
        text: i18n.tr("Scroll with two fingers")
        control: CheckBox {
            property bool serverChecked: touchPad.twoFingerScroll
            onServerCheckedChanged: checked = serverChecked
            Component.onCompleted: checked = serverChecked
            onTriggered: touchPad.twoFingerScroll = checked
        }
        showDivider: false
    }

    ListItem.ItemSelector {
        anchors {
            left: parent.left
            right: parent.right
        }
        activeFocusOnPress: false
        //expanded: true
        text: i18n.tr("Ignore touchpad when:")
        model: [i18n.tr("Typing"),
                i18n.tr("A mouse is connected")]
        multiSelection: true
        onDelegateClicked: {
            console.warn("selected: " + ViewItems.selectedIndices);
            for (var item in model) {
                console.warn(item);
             }
        }
    }
}
