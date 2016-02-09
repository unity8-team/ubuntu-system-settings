/*
 * Copyright 2015 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

pragma Singleton

import QtQuick 2.4
import QtSystemInfo 5.5

Item {
    id: root
    readonly property alias mice: priv.miceCount
    readonly property alias touchpads: priv.touchpadCount
    readonly property alias keyboards: priv.keyboardCount

    property alias inputInfo: inputInfo

    QtObject {
        id: priv

        property var mice: []
        property var touchpads: []
        property var keyboards: []

        property int miceCount: 0
        property int touchpadCount: 0
        property int keyboardCount: 0

        function addMouse(devicePath) {
            mice.push(devicePath);
            miceCount++;
        }

        function addTouchPad(devicePath) {
            touchpads.push(devicePath);
            touchpadCount++;
        }

        function addKeyboard(devicePath) {
            keyboards.push(devicePath);
            keyboardCount++;
        }

        function removeDevice(devicePath) {
            for (var i = 0; i < priv.mice.length; i++) {
                if (priv.mice[i] == devicePath) {
                    priv.mice.splice(i, 1);
                    priv.miceCount--;
                }
            }
            for (var i = 0; i < priv.touchpads.length; i++) {
                if (priv.touchpads[i] == devicePath) {
                    priv.touchpads.splice(i, 1);
                    priv.touchpadCount--;
                }
            }
            for (var i = 0; i < priv.keyboards.length; i++) {
                if (priv.keyboards[i] == devicePath) {
                    priv.keyboards.splice(i, 1);
                    priv.keyboardCount--;
                }
            }
        }
    }

    InputDeviceModel {
        id: inputInfo
        filter: InputInfo.Keyboard | InputInfo.Mouse | InputInfo.TouchScreen | InputInfo.TouchScreen
        objectName: "inputDeviceInfo"
        onAdded: {
            var device = inputDevice;
            if (device === null) {
                return;
            }
            var devicePath = device.devicePath;

            var hasMouse = device.type & InputInfo.Mouse;
            var hasTouchpad = device.type & InputInfo.TouchPad;
            var hasKeyboard = device.type & InputInfo.Keyboard;

            if (hasMouse) {
                priv.addMouse(devicePath);
            } else if (hasTouchpad) {
                priv.addTouchPad(devicePath);
            } else if (hasKeyboard) {
                // Only accepting keyboards that do not claim to be a mouse too
                // This will be a bit buggy for real hybrid devices, but doesn't
                // fall for Microsoft mice that claim to be Keyboards too.
                priv.addKeyboard(devicePath)
            }
        }
        onRemoved: {
            priv.removeDevice(devicePath)
        }
    }
}
