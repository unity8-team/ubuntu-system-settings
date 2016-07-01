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
            height: units.gu(6)
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
        id: requestInstallSpy
        signalName: "requestInstall"
    }

    SignalSpy {
        id: resumeSignalSpy
        signalName: "resume"
    }

    UbuntuTestCase {
        name: "GlobalTest"
        when: windowShown

        property var instance: null

        function init () {
            instance = glob.createObject(testRoot, {});

            stopSignalSpy.target = instance;
            pauseSignalSpy.target = instance;
            requestInstallSpy.target = instance;
            resumeSignalSpy.target = instance;
        }

        function cleanup () {
            stopSignalSpy.clear();
            pauseSignalSpy.clear();
            requestInstallSpy.clear();
            resumeSignalSpy.clear();

            instance.destroy();
            instance = null;
        }

        function test_checkInProgress() {
            instance.updatesCount = 2;
            instance.online = true;
            instance.status = SystemUpdate.StatusCheckingAllUpdates;

            compare(instance.hidden, false, "global was not visible");
        }

        function test_stop() {
            instance.updatesCount = 2;
            instance.online = true;
            instance.status = SystemUpdate.StatusCheckingAllUpdates;

            var stop = findChild(instance, "updatesGlobalStopButton");
            compare(stop.visible, true);
            mouseClick(stop, stop.width / 2, stop.height / 2);
            stopSignalSpy.wait();
        }

        function test_checkIdle() {
            instance.updatesCount = 2;
            instance.online = true;
            instance.status = SystemUpdate.StatusIdle;

            compare(instance.hidden, false, "global was not visible");
        }

        function test_installApps() {
            instance.updatesCount = 2;
            instance.online = true;
            instance.status = SystemUpdate.StatusIdle;

            var install = findChild(instance, "updatesGlobalInstallButton");
            compare(install.visible, true);
            compare(install.text, i18n.tr("Install %1 update", "Install %1 updates", 2).arg(2));
            mouseClick(install, install.width / 2, install.height / 2);
            requestInstallSpy.wait();
        }

        function test_installWithRestart() {
            instance.updatesCount = 2;
            instance.online = true;
            instance.status = SystemUpdate.StatusIdle;
            instance.requireRestart = true;

            var install = findChild(instance, "updatesGlobalInstallButton");
            compare(install.visible, true);
            compare(install.text, i18n.tr("Install %1 update…", "Install %1 updates…", 2).arg(2));
            mouseClick(install, install.width / 2, install.height / 2);
            requestInstallSpy.wait();
        }

        function test_singleUpdate() {
            instance.updatesCount = 1;
            instance.online = true;
            instance.status = SystemUpdate.StatusIdle;

            compare(instance.hidden, true, "global was visible for single update");
        }

        function test_batchMode() {
            instance.updatesCount = 2;
            instance.online = true;
            instance.status = SystemUpdate.StatusBatchMode;

            compare(instance.hidden, false, "global was hidden in batchmode");
        }

        function test_pause() {
            instance.updatesCount = 2;
            instance.online = true;
            instance.status = SystemUpdate.StatusBatchMode;

            var pause = findChild(instance, "updatesGlobalPauseButton");
            compare(pause.visible, true);
            mouseClick(pause, pause.width / 2, pause.height / 2);
            pauseSignalSpy.wait();
        }

        function test_resume() {
            instance.updatesCount = 2;
            instance.online = true;
            instance.status = SystemUpdate.StatusBatchModePaused;

            var resume = findChild(instance, "updatesGlobalResumeButton");
            compare(resume.visible, true);
            mouseClick(resume, resume.width / 2, resume.height / 2);
            resumeSignalSpy.wait();
        }

        function test_visibility_data() {
            return [
                {
                    tag: "idle",
                    status: SystemUpdate.StatusIdle, online: true, updatesCount: 0, hidden: true
                },
                {
                    tag: "idle, no network",
                    status: SystemUpdate.StatusIdle, online: false, updatesCount: 0, hidden: true
                },
                {
                    tag: "idle, one update",
                    status: SystemUpdate.StatusIdle, online: true, updatesCount: 1, hidden: true
                },
                {
                    tag: "idle, two updates",
                    status: SystemUpdate.StatusIdle, online: true, updatesCount: 2, hidden: false
                },
                {
                    tag: "idle, two updates, no network",
                    status: SystemUpdate.StatusIdle, online: false, updatesCount: 2, hidden: true
                },
                {
                    tag: "checking clicks, no network, no updates",
                    status: SystemUpdate.StatusCheckingClickUpdates, online: false, updatesCount: 0, hidden: true
                },
                {
                    tag: "checking clicks, no network, updates",
                    status: SystemUpdate.StatusCheckingClickUpdates, online: false, updatesCount: 2, hidden: true
                },
                {
                    tag: "checking clicks, network, no updates",
                    status: SystemUpdate.StatusCheckingClickUpdates, online: true, updatesCount: 0, hidden: false
                },
                {
                    tag: "checking clicks, network, one update",
                    status: SystemUpdate.StatusCheckingClickUpdates, online: true, updatesCount: 1, hidden: false
                },
                {
                    tag: "checking clicks, network, updates",
                    status: SystemUpdate.StatusCheckingClickUpdates, online: true, updatesCount: 2, hidden: false
                },
                {
                    tag: "checking image, no network, no updates",
                    status: SystemUpdate.StatusCheckingSystemUpdates, online: false, updatesCount: 0, hidden: true
                },
                {
                    tag: "checking image, no network, updates",
                    status: SystemUpdate.StatusCheckingSystemUpdates, online: false, updatesCount: 2, hidden: true
                },
                {
                    tag: "checking image, network, no updates",
                    status: SystemUpdate.StatusCheckingSystemUpdates, online: true, updatesCount: 0, hidden: false
                },
                {
                    tag: "checking image, network, one update",
                    status: SystemUpdate.StatusCheckingSystemUpdates, online: true, updatesCount: 1, hidden: false
                },
                {
                    tag: "checking image, network, updates",
                    status: SystemUpdate.StatusCheckingSystemUpdates, online: true, updatesCount: 2, hidden: false
                },
                {
                    tag: "checking all, no network, no updates",
                    status: SystemUpdate.StatusCheckingAllUpdates, online: false, updatesCount: 0, hidden: true
                },
                {
                    tag: "checking all, no network, updates",
                    status: SystemUpdate.StatusCheckingAllUpdates, online: false, updatesCount: 2, hidden: true
                },
                {
                    tag: "checking all, network, no updates",
                    status: SystemUpdate.StatusCheckingAllUpdates, online: true, updatesCount: 0, hidden: false
                },
                {
                    tag: "checking all, network, one update",
                    status: SystemUpdate.StatusCheckingAllUpdates, online: true, updatesCount: 1, hidden: false
                },
                {
                    tag: "checking all, network, updates",
                    status: SystemUpdate.StatusCheckingAllUpdates, online: true, updatesCount: 2, hidden: false
                },
                {
                    tag: "batch mode, no network, no updates",
                    status: SystemUpdate.StatusBatchMode, online: false, updatesCount: 0, hidden: true
                },
                {
                    tag: "batch mode, no network, updates",
                    status: SystemUpdate.StatusBatchMode, online: false, updatesCount: 2, hidden: true
                },
                {
                    tag: "batch mode, network, no updates",
                    status: SystemUpdate.StatusBatchMode, online: true, updatesCount: 0, hidden: false
                },
                {
                    tag: "batch mode, network, one update",
                    status: SystemUpdate.StatusBatchMode, online: true, updatesCount: 1, hidden: false
                },
                {
                    tag: "batch mode, network, updates",
                    status: SystemUpdate.StatusBatchMode, online: true, updatesCount: 2, hidden: false
                },
                {
                    tag: "batch mode (paused), no network, no updates",
                    status: SystemUpdate.StatusBatchModePaused, online: false, updatesCount: 0, hidden: true
                },
                {
                    tag: "batch mode (paused), no network, updates",
                    status: SystemUpdate.StatusBatchModePaused, online: false, updatesCount: 2, hidden: true
                },
                {
                    tag: "batch mode (paused), network, no updates",
                    status: SystemUpdate.StatusBatchModePaused, online: true, updatesCount: 0, hidden: false
                },
                {
                    tag: "batch mode (paused), network, one update",
                    status: SystemUpdate.StatusBatchModePaused, online: true, updatesCount: 1, hidden: false
                },
                {
                    tag: "batch mode (paused), network, updates",
                    status: SystemUpdate.StatusBatchModePaused, online: true, updatesCount: 2, hidden: false
                },
                {
                    tag: "network error, no network, no updates",
                    status: SystemUpdate.StatusNetworkError, online: false, updatesCount: 0, hidden: true
                },
                {
                    tag: "network error, no network, updates",
                    status: SystemUpdate.StatusNetworkError, online: false, updatesCount: 2, hidden: true
                },
                {
                    tag: "network error, network, no updates",
                    status: SystemUpdate.StatusNetworkError, online: true, updatesCount: 0, hidden: true
                },
                {
                    tag: "network error, network, one update",
                    status: SystemUpdate.StatusNetworkError, online: true, updatesCount: 1, hidden: true
                },
                {
                    tag: "network error, network, updates",
                    status: SystemUpdate.StatusNetworkError, online: true, updatesCount: 2, hidden: true
                },
                {
                    tag: "server error, no network, no updates",
                    status: SystemUpdate.StatusServerError, online: false, updatesCount: 0, hidden: true
                },
                {
                    tag: "server error, no network, updates",
                    status: SystemUpdate.StatusServerError, online: false, updatesCount: 2, hidden: true
                },
                {
                    tag: "server error, network, no updates",
                    status: SystemUpdate.StatusServerError, online: true, updatesCount: 0, hidden: true
                },
                {
                    tag: "server error, network, one update",
                    status: SystemUpdate.StatusServerError, online: true, updatesCount: 1, hidden: true
                },
                {
                    tag: "server error, network, updates",
                    status: SystemUpdate.StatusServerError, online: true, updatesCount: 2, hidden: true
                },
            ]
        }

        function test_visibility(data) {
            instance.status = data.status;
            instance.online = data.online;
            instance.updatesCount = data.updatesCount;
            compare(instance.hidden, data.hidden);
        }
    }
}
