/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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
 *
 * This is testing of the Global component, which is the spinner as well
 * as the “Update all/Pause all” area.
 */
import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.Update 1.0
import Ubuntu.Test 0.1

import Source 1.0

Item {
    id: testRoot
    width: units.gu(50)
    height: units.gu(90)

    Component {
        id: glob

        Global {
            anchors {
                left: parent.left;
                right: parent.right
                margins: units.gu(2)
            }
            width: testRoot.width
            height: hidden ? 0 : units.gu(6)
        }
    }

    SignalSpy {
        id: stopSignalSpy
        signalName: "stop"
    }


    SignalSpy {
        id: pauseSignalSpy
        signalName: "pause"
    }


    SignalSpy {
        id: installSignalSpy
        signalName: "install"
    }

    UbuntuTestCase {
        name: "GlobalTest"
        when: windowShown

        property var cGlob: null

        function init () {
            cGlob = glob.createObject(testRoot, {});

            stopSignalSpy.target = cGlob;
            pauseSignalSpy.target = cGlob;
            installSignalSpy.target = cGlob;
        }

        function cleanup () {
            stopSignalSpy.clear();
            pauseSignalSpy.clear();
            installSignalSpy.clear();

            cGlob.destroy();
            cGlob = null;
        }

        function test_checkInProgress() {
            cGlob.updatesCount = 2;
            cGlob.online = true;
            cGlob.managerStatus = UpdateManager.CheckingAllUpdates;

            compare(cGlob.hidden, false, "global was not visible");
            wait(3000)
        }

        function test_stop() {
            cGlob.updatesCount = 2;
            cGlob.online = true;
            cGlob.managerStatus = UpdateManager.CheckingAllUpdates;

            var stop = findChild(cGlob, "updatesGlobalStopButton");
            compare(stop.visible, true);
            mouseClick(stop, stop.width / 2, stop.height / 2);
            stopSignalSpy.wait();
            wait(3000)
        }

        function test_checkIdle() {
            cGlob.updatesCount = 2;
            cGlob.online = true;
            cGlob.managerStatus = UpdateManager.Idle;

            compare(cGlob.hidden, false, "global was not visible");
            wait(3000)
        }

        function test_installApps() {
            cGlob.updatesCount = 2;
            cGlob.online = true;
            cGlob.managerStatus = UpdateManager.Idle;

            var install = findChild(cGlob, "updatesGlobalInstallButton");
            compare(install.visible, true);
            compare(install.text, i18n.tr("Install %1 update", "Install %1 updates", 2).arg(2));
            mouseClick(install, install.width / 2, install.height / 2);
            installSignalSpy.wait();
            wait(3000)
        }

        function test_installWithRestart() {
            cGlob.updatesCount = 2;
            cGlob.online = true;
            cGlob.managerStatus = UpdateManager.Idle;
            cGlob.requireRestart = true;

            var install = findChild(cGlob, "updatesGlobalInstallButton");
            compare(install.visible, true);
            compare(install.text, i18n.tr("Install %1 update…", "Install %1 updates…", 2).arg(2));
            mouseClick(install, install.width / 2, install.height / 2);
            installSignalSpy.wait();
            wait(3000)
        }

        function test_singleUpdate() {
            cGlob.updatesCount = 1;
            cGlob.online = true;
            cGlob.managerStatus = UpdateManager.Idle;

            compare(cGlob.hidden, true, "global was visible for single update");
            wait(3000)
        }

        function test_batchMode() {
            cGlob.updatesCount = 2;
            cGlob.online = true;
            cGlob.managerStatus = UpdateManager.BatchMode;

            compare(cGlob.hidden, false, "global was hidden in batchmode");
            wait(3000)
        }

        function test_pause() {
            cGlob.updatesCount = 2;
            cGlob.online = true;
            cGlob.managerStatus = UpdateManager.BatchMode;

            var pause = findChild(cGlob, "updatesGlobalPauseButton");
            compare(pause.visible, true);
            mouseClick(pause, pause.width / 2, pause.height / 2);
            pauseSignalSpy.wait();
            wait(3000)
        }
    }
}
