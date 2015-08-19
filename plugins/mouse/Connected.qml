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
 
import GSettings 1.0
import QtQuick 2.0
import SystemSettings 1.0
import Ubuntu.Components 1.2
import Ubuntu.Components.ListItems 1.2 as ListItem
import Ubuntu.Settings.Menus 0.1 as Menus
import Ubuntu.Settings.Components 0.1 as USC
import "Components/UnityInputInfo"

Column {
    anchors.left: parent.left
    anchors.right: parent.right
    
    GSettings {
        id: settings
        schema.id: "com.ubuntu.touch.system-settings"
    }
    
    Column {
        anchors.left: parent.left
        anchors.right: parent.right
        visible: UnityInputInfo.mice > 0
        
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
            value: settings.mouseCursorSpeed
            live: true
            property real serverValue: enabled ? settings.mouseCursorSpeed : 0.0
            USC.ServerPropertySynchroniser {
                userTarget: mouseMoveSpeed
                userProperty: "value"
                serverTarget: mouseMoveSpeed
                serverProperty: "serverValue"
                maximumWaitBufferInterval: 16
                
                onSyncTriggered: settings.mouseCursorSpeed = value
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
            value: settings.mouseScrollSpeed
            live: true
            property real serverValue: enabled ? settings.mouseScrollSpeed : 0.0
            USC.ServerPropertySynchroniser {
                userTarget: mouseScrollSpeed
                userProperty: "value"
                serverTarget: mouseScrollSpeed
                serverProperty: "serverValue"
                maximumWaitBufferInterval: 16
                
                onSyncTriggered: settings.mouseScrollSpeed = value
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
            id: mouseDoubleClickSpeed
            objectName: "mouseDoubleClickSpeed"
            function formatValue(v) { return v.toFixed(2) }
            minimumValue: 100
            maximumValue: 1000
            value: settings.mouseDoubleClickSpeed
            live: true
            property real serverValue: enabled ? settings.mouseDoubleClickSpeed : 0.0
            USC.ServerPropertySynchroniser {
                userTarget: mouseDoubleClickSpeed
                userProperty: "value"
                serverTarget: mouseDoubleClickSpeed
                serverProperty: "serverValue"
                maximumWaitBufferInterval: 16
                
                onSyncTriggered: settings.mouseDoubleClickSpeed = value
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
            doubleTapSpeed: settings.mouseDoubleClickSpeed
        }


        PrimaryButtonSelector {
            id: mousePrimarySelector
            anchors { 
                left: parent.left
                right: parent.right
            }
            height: childrenRect.height
            buttonEnum: settings.mousePrimaryButton
            onButtonEnumChanged: {
                console.warn("buttonEnum: " + buttonEnum);
                settings.mousePrimaryButton = buttonEnum;
            }
            Binding {
                target: mousePrimarySelector
                property: "buttonEnum"
                value: settings.mousePrimaryButton
            }
        }
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right
        visible: UnityInputInfo.touchpads > 0

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
            value: settings.touchpadCursorSpeed
            live: true
            property real serverValue: enabled ? settings.touchpadCursorSpeed : 0.0
            USC.ServerPropertySynchroniser {
                userTarget: touchMoveSpeed
                userProperty: "value"
                serverTarget: touchMoveSpeed
                serverProperty: "serverValue"
                maximumWaitBufferInterval: 16
                
                onSyncTriggered: settings.touchpadCursorSpeed = value
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
            value: settings.touchpadScrollSpeed
            live: true
            property real serverValue: enabled ? settings.touchpadScrollSpeed : 0.0
            USC.ServerPropertySynchroniser {
                userTarget: touchScrollSpeed
                userProperty: "value"
                serverTarget: touchScrollSpeed
                serverProperty: "serverValue"
                maximumWaitBufferInterval: 16
                
                onSyncTriggered: settings.touchpadScrollSpeed = value
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
            value: settings.touchpadDoubleClickSpeed
            live: true
            property real serverValue: enabled ? settings.touchpadDoubleClickSpeed : 0.0
            USC.ServerPropertySynchroniser {
                userTarget: touchClickSpeed
                userProperty: "value"
                serverTarget: touchClickSpeed
                serverProperty: "serverValue"
                maximumWaitBufferInterval: 16
                
                onSyncTriggered: settings.touchpadDoubleClickSpeed = value
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
            doubleTapSpeed: settings.touchpadDoubleClickSpeed
        }

        PrimaryButtonSelector {
            id: touchpadPrimarySelector
            anchors { 
                left: parent.left
                right: parent.right
            }
            height: childrenRect.height
            buttonEnum: settings.touchpadPrimaryButton
            onButtonEnumChanged: {
                console.warn("buttonEnum: " + buttonEnum);
                settings.touchpadPrimaryButton = buttonEnum;
            }
            Binding {
                target: touchpadPrimarySelector
                property: "buttonEnum"
                value: settings.touchpadPrimaryButton
            }
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(2)
                rightMargin: units.gu(2)
                topMargin: units.gu(2)
                bottomMargin: units.gu(2)
            }
            spacing: units.gu(1)
            Row {
                spacing: units.gu(1)
                CheckBox {
                    property bool serverChecked: settings.touchpadTapToClick
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.touchpadTapToClick = checked
                }
                Label {
                    text: i18n.tr("Tap to click")
                }
            }
            Row {
                spacing: units.gu(1)
                CheckBox {
                    property bool serverChecked: settings.touchpadTwoFingerScroll
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.touchpadTwoFingerScroll = checked
                }
                Label {
                    text: i18n.tr("Scroll with two fingers")
                }
            }
        }

        ListItem.Header {
            text: i18n.tr("Ignore touchpad when:")
        }
        Column {
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(2)
                rightMargin: units.gu(2)
                topMargin: units.gu(2)
                bottomMargin: units.gu(2)
            }
            spacing: units.gu(1)
            Row {
                spacing: units.gu(1)
                CheckBox {
                    property bool serverChecked: settings.touchpadDisableWhileTyping
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.touchpadDisableWhileTyping = checked
                }
                Label { text: i18n.tr("Typing") }
            }
            Row {
                spacing: units.gu(1)
                CheckBox {
                    property bool serverChecked: settings.touchpadDisableWithMouse
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: settings.touchpadDisableWithMouse = checked
                }
                Label { text: i18n.tr("A mouse is connected") }
            }
        }
    }
}
