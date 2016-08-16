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
import QtTest 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.SystemSettings.Wifi 1.0
import Ubuntu.Test 0.1

import WifiSource 1.0

Item {
    id: testRoot
    width: units.gu(50)
    height: units.gu(90)

    OtherNetwork {
        id: otherNetworkDialog
    }

    UbuntuTestCase {
        name: "OtherNetworkTest"
        when: windowShown

        property var instance

        function init() {
            instance = PopupUtils.open(otherNetworkDialog);
        }

        function cleanup() {
            if (instance) {
                instance.destroy();
                tryCompareFunction(function () {
                    return !!findChild(testRoot, "otherNetworkDialog");
                }, false);
            }
        }

        function setSelectedIndex(objectName, index) {
            findChild(instance, objectName).selectedIndex = index;
        }

        function setName(name) {
            findChild(instance, "networkname").text = name;
        }

        function setSecurity(index) {
            setSelectedIndex("securityList", index);
        }

        function setAuth(index) {
            setSelectedIndex("authList", index);
        }

        function setP2auth(index) {
            setSelectedIndex("p2authList", index);
        }

        function setPeapVersion(index) {
            setSelectedIndex("peapVersionList", index);
        }

        function setUsername(username) {
            findChild(instance, "username").text = username;
        }

        function setPassword(password) {
            findChild(instance, "password").text = password;
        }

        function cancel() {
            mouseClick(findChild(instance, "cancel"));
        }

        function connect() {
            mouseClick(findChild(instance, "connect"));
        }

        function test_connect_to_hidden_network() {
            setName("test_ap");
            setPassword("abcdefgh");
            connect();
            DbusHelper.mockDeviceStateChanged(100, 0);

            tryCompare(instance, "state", "SUCCEEDED");
            compare(DbusHelper.getConnectArguments()["ssid"], "test_ap");
            compare(DbusHelper.getConnectArguments()["password"], ["abcdefgh", "false"]);
        }

        function test_connect_to_nonexistant_hidden_network() {
            setName("n/a");
            setPassword("abcdefgh");
            connect();
            DbusHelper.mockDeviceStateChanged(120, 53);
            tryCompare(instance, "state", "FAILED");

            compare(instance.text, i18n.tr('The Wi-Fi network could not be found'));
        }

        function test_connect_to_hidden_network_using_secrets() {
            setName("test_ap");
            setPassword("abcdefgh");
            setSecurity(1);
            connect();
            DbusHelper.mockDeviceStateChanged(100, 0);

            compare(DbusHelper.getConnectArguments()["security"], 1);
        }

        function test_connect_to_hidden_network_using_incorrect_secrets() {
            setName("test_ap");
            setPassword("wrongpassword");
            setSecurity(1);
            connect();
            DbusHelper.mockDeviceStateChanged(120, 7);

            compare(instance.text, i18n.tr('Your authentication details were incorrect'));
        }

        function test_connect_to_hidden_network_then_cancel() {
            setName("test_ap");
            setPassword("abcdefgh");
            connect();
            cancel();

            verify(DbusHelper.getForgetActiveDeviceCalled());
        }

        function test_connect_to_eduroam() {
            setName("eduroam");
            setUsername("student");
            setPassword("abcdefgh");
            setSecurity(2); // wpa-ep
            setAuth(4); // PEAP
            setP2auth(1); // MSCHAPv2
            connect();
            DbusHelper.mockDeviceStateChanged(100, 0);

            compare(DbusHelper.getConnectArguments(), {
                "auth": 4,
                "certs": ["", "", "", "", "1", "2"],
                "p2auth": 1,
                "password": ["abcdefgh", "false"],
                "security": 2,
                "ssid": "eduroam",
                "usernames": ["student", ""]
            });
        }
    }
}
