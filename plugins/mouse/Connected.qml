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
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.Settings.Menus 0.1 as Menus
import Ubuntu.Settings.Components 0.1 as USC
import Ubuntu.SystemSettings.Mouse 1.0

Column {
    anchors {
        left: parent.left
        right: parent.right
    }
    height: childrenRect.height
    
    UbuntuMousePanel {
        id: backend
    }
    
    Column {
        anchors {
            left: parent.left
            right: parent.right
        }
        visible: miceModel.count > 0

        SectionHeader {
            text: i18n.tr("Mouse")
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: 0
            height: childrenRect.height

            Menus.SliderMenu {
                /* FIXME: No appropriate icons exist yet, also SliderMenu lacks
                          support for text labels on the ends. */
                text: i18n.tr("Cursor speed:")
                anchors { 
                    left: parent.left
                    right: parent.right
                }
                id: mouseMoveSpeed
                objectName: "mouseMoveSpeed"
                function formatValue(v) { return v.toFixed(2) }
                minimumValue: 0.0
                maximumValue: 1.0
                value: backend.mouseCursorSpeed
                live: true
                property real serverValue: enabled ? backend.mouseCursorSpeed : 0.0
                USC.ServerPropertySynchroniser {
                    userTarget: mouseMoveSpeed
                    userProperty: "value"
                    serverTarget: mouseMoveSpeed
                    serverProperty: "serverValue"
                    maximumWaitBufferInterval: 16
                    
                    onSyncTriggered: backend.mouseCursorSpeed = value
                }
            }
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: 0
            height: childrenRect.height

            Menus.SliderMenu {
                /* FIXME: No appropriate icons exist yet, also SliderMenu lacks
                          support for text labels on the ends. */
                text: i18n.tr("Wheel scrolling speed:")
                anchors { 
                    left: parent.left
                    right: parent.right
                }
                id: mouseScrollSpeed
                objectName: "mouseMoveSpeed"
                function formatValue(v) { return v.toFixed(2) }
                minimumValue: 0.0
                maximumValue: 3.0
                value: backend.mouseScrollSpeed
                live: true
                property real serverValue: enabled ? backend.mouseScrollSpeed : 0.0
                USC.ServerPropertySynchroniser {
                    userTarget: mouseScrollSpeed
                    userProperty: "value"
                    serverTarget: mouseScrollSpeed
                    serverProperty: "serverValue"
                    maximumWaitBufferInterval: 16
                    
                    onSyncTriggered: backend.mouseScrollSpeed = value
                }
            }
        }
        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: 0
            height: childrenRect.height
            visible: showAllUI

            Menus.SliderMenu {
                /* FIXME: No appropriate icons exist yet, also SliderMenu lacks
                          support for text labels on the ends. */
                text: i18n.tr("Double-click speed:")
                anchors { 
                    left: parent.left
                    right: parent.right
                }
                id: mouseDoubleClickSpeed
                objectName: "mouseDoubleClickSpeed"
                function formatValue(v) { return v.toFixed(2) }
                minimumValue: 100
                maximumValue: 1000
                value: backend.mouseDoubleClickSpeed
                live: true
                property int serverValue: enabled ? backend.mouseDoubleClickSpeed : 0
                USC.ServerPropertySynchroniser {
                    userTarget: mouseDoubleClickSpeed
                    userProperty: "value"
                    serverTarget: mouseDoubleClickSpeed
                    serverProperty: "serverValue"
                    maximumWaitBufferInterval: 16
                    
                    onSyncTriggered: backend.mouseDoubleClickSpeed = value
                }
            }
        }
        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(3)
            height: childrenRect.height
            visible: showAllUI

            ItemTitle {
                text: i18n.tr("Test double-click:")
                showDivider: false
            }

            TapArea {
                anchors { 
                    left: parent.left
                    right: parent.right
                    leftMargin: units.gu(2)
                    rightMargin: units.gu(2)
                    topMargin: units.gu(2)
                }
                height: units.gu(5)
                doubleTapSpeed: backend.mouseDoubleClickSpeed
            }    
        }

        PrimaryButtonSelector {
            id: mousePrimarySelector
            anchors { 
                left: parent.left
                right: parent.right
            }
            height: childrenRect.height + units.gu(2)
            selected: backend.mousePrimaryButton
            onSelectedChanged: {
                backend.mousePrimaryButton = selected;
            }
            Binding {
                target: mousePrimarySelector
                property: "selected"
                value: backend.mousePrimaryButton
            }
        }
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            topMargin: units.gu(2)
        }
        visible: touchpadsModel.count > 0
        spacing: units.gu(0.1)

        SectionHeader {
            text: i18n.tr("Touchpad")
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: 0
            height: childrenRect.height

            Menus.SliderMenu {
                /* FIXME: No appropriate icons exist yet, also SliderMenu lacks
                          support for text labels on the ends. */
                text: i18n.tr("Cursor speed:")
                anchors { 
                    left: parent.left
                    right: parent.right
                }
                id: touchMoveSpeed
                objectName: "touchMoveSpeed"
                function formatValue(v) { return v.toFixed(2) }
                minimumValue: 0.0
                maximumValue: 1.0
                value: backend.touchpadCursorSpeed
                live: true
                property real serverValue: enabled ? backend.touchpadCursorSpeed : 0.0
                USC.ServerPropertySynchroniser {
                    userTarget: touchMoveSpeed
                    userProperty: "value"
                    serverTarget: touchMoveSpeed
                    serverProperty: "serverValue"
                    maximumWaitBufferInterval: 16
                    
                    onSyncTriggered: backend.touchpadCursorSpeed = value
                }
            }
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: 0
            height: childrenRect.height

            Menus.SliderMenu {
                /* FIXME: No appropriate icons exist yet, also SliderMenu lacks
                          support for text labels on the ends. */
                text: i18n.tr("Finger scrolling speed:")
                anchors { 
                    left: parent.left
                    right: parent.right
                }
                id: touchScrollSpeed
                objectName: "touchScrollSpeed"
                function formatValue(v) { return v.toFixed(2) }
                minimumValue: 0.0
                maximumValue: 3.0
                value: backend.touchpadScrollSpeed
                live: true
                property real serverValue: enabled ? backend.touchpadScrollSpeed : 0.0
                USC.ServerPropertySynchroniser {
                    userTarget: touchScrollSpeed
                    userProperty: "value"
                    serverTarget: touchScrollSpeed
                    serverProperty: "serverValue"
                    maximumWaitBufferInterval: 16
                    
                    onSyncTriggered: backend.touchpadScrollSpeed = value
                }
            }
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: 0
            height: childrenRect.height
            visible: showAllUI

            Menus.SliderMenu {
                /* FIXME: No appropriate icons exist yet, also SliderMenu lacks
                          support for text labels on the ends. */
                text: i18n.tr("Double-click speed:")
                anchors { 
                    left: parent.left
                    right: parent.right
                }
                id: touchClickSpeed
                objectName: "touchClickSpeed"
                function formatValue(v) { return v.toFixed(2) }
                minimumValue: 100
                maximumValue: 1000
                value: backend.touchpadDoubleClickSpeed
                live: true
                property int serverValue: enabled ? backend.touchpadDoubleClickSpeed : 0
                USC.ServerPropertySynchroniser {
                    userTarget: touchClickSpeed
                    userProperty: "value"
                    serverTarget: touchClickSpeed
                    serverProperty: "serverValue"
                    maximumWaitBufferInterval: 16
                    
                    onSyncTriggered: backend.touchpadDoubleClickSpeed = value
                }
            }
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(3)
            height: childrenRect.height
            visible: showAllUI
            ItemTitle {
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
                doubleTapSpeed: backend.touchpadDoubleClickSpeed
            }
        }

        PrimaryButtonSelector {
            id: touchpadPrimarySelector
            anchors { 
                left: parent.left
                right: parent.right
            }
            height: childrenRect.height + units.gu(1)
            selected: backend.touchpadPrimaryButton
            onSelectedChanged: {
                backend.touchpadPrimaryButton = selected;
            }
            Binding {
                target: touchpadPrimarySelector
                property: "selected"
                value: backend.touchpadPrimaryButton
            }
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(2)
                rightMargin: units.gu(2)
                topMargin: units.gu(2)
            }
            spacing: units.gu(1)
            Row {
                spacing: units.gu(1)
                CheckBox {
                    property bool serverChecked: backend.touchpadTapToClick
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: backend.touchpadTapToClick = checked
                }
                Label {
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    text: i18n.tr("Tap to click")
                }
            }
            Row {
                spacing: units.gu(1)
                CheckBox {
                    property bool serverChecked: backend.touchpadTwoFingerScroll
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: backend.touchpadTwoFingerScroll = checked
                }
                Label {
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    text: i18n.tr("Scroll with two fingers")
                }
            }
        }

        ListItem.Header {
            anchors.topMargin: units.gu(2)
            text: i18n.tr("Ignore touchpad when:")
        }
        Column {
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(2)
                rightMargin: units.gu(2)
                topMargin: units.gu(2)
            }
            spacing: units.gu(1)
            Row {
                spacing: units.gu(1)
                CheckBox {
                    property bool serverChecked: backend.touchpadDisableWhileTyping
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: backend.touchpadDisableWhileTyping = checked
                }
                Label {
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    text: i18n.tr("Typing")
                }
            }
            Row {
                spacing: units.gu(1)
                CheckBox {
                    property bool serverChecked: backend.touchpadDisableWithMouse
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: backend.touchpadDisableWithMouse = checked
                }
                Label {
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    text: i18n.tr("A mouse is connected")
                }
            }
        }
    }
}
