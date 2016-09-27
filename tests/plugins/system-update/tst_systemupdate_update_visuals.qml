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
        id: update

        UpdateDelegate {
            anchors { left: parent.left; right: parent.right }
            onDownload: updateState = Update.StateDownloading
        }
    }

    Flickable {
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height

        Column {
            id: col
            anchors { left: parent.left; right: parent.right }
        }
    }


    UbuntuTestCase {
        name: "UpdateTest"
        when: windowShown

        function test_all_data() {
            return [
                {
                    datas: [
                        {
                            name: "StateAvailable",
                            kind: Update.KindImage,
                            updateState: Update.StateAvailable,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            iconUrl: "file:///usr/share/icons/suru/places/scalable/distributor-logo.svg"
                        },
                        {
                            name: "StateUnavailable (with a really long name too so to test size constraints)",
                            kind: Update.KindClick,
                            updateState: Update.StateUnavailable,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "https://upload.wikimedia.org/wikipedia/en/8/80/Wikipedia-logo-v2.svg"
                        },
                        {
                            name: "StateQueuedForDownload",
                            kind: Update.KindClick,
                            updateState: Update.StateQueuedForDownload,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "file:///usr/share/icons/suru/apps/sources/online-accounts-google.svg"
                        },
                        {
                            name: "StateDownloading",
                            kind: Update.KindClick,
                            updateState: Update.StateDownloading,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "file:///usr/share/icons/suru/apps/sources/online-accounts-flickr.svg"
                        },
                        {
                            name: "StateDownloadingAutomatically",
                            kind: Update.KindClick,
                            updateState: Update.StateDownloadingAutomatically,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "file:///usr/share/icons/suru/apps/sources/online-accounts-yahoo.svg"
                        },
                        {
                            name: "StateDownloadPaused",
                            kind: Update.KindClick,
                            updateState: Update.StateDownloadPaused,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "file:///usr/share/icons/suru/apps/sources/online-accounts-twitter.svg"
                        },
                        {
                            name: "StateAutomaticDownloadPaused",
                            kind: Update.KindClick,
                            updateState: Update.StateAutomaticDownloadPaused,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "file:///usr/share/icons/suru/apps/sources/online-accounts-twitter.svg"
                        },
                        {
                            name: "StateInstalling",
                            kind: Update.KindClick,
                            updateState: Update.StateInstalling,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "file:///usr/share/icons/suru/apps/sources/online-accounts-twitter.svg"
                        },
                        {
                            name: "StateInstallingAutomatically",
                            kind: Update.KindClick,
                            updateState: Update.StateInstallingAutomatically,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "file:///usr/share/icons/suru/apps/sources/online-accounts-twitter.svg"
                        },
                        {
                            name: "StateInstallPaused",
                            kind: Update.KindClick,
                            updateState: Update.StateInstallPaused,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "file:///usr/share/icons/suru/apps/sources/online-accounts-twitter.svg"
                        },
                        {
                            name: "StateInstallFinished",
                            kind: Update.KindClick,
                            updateState: Update.StateInstallFinished,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "file:///usr/share/icons/suru/apps/sources/online-accounts-twitter.svg"
                        },
                        {
                            name: "StateInstalled",
                            kind: Update.KindClick,
                            updateState: Update.StateInstalled,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "file:///usr/share/icons/suru/apps/sources/online-accounts-twitter.svg",
                            updatedAt: new Date()
                        },
                        {
                            name: "StateDownloaded",
                            kind: Update.KindClick,
                            updateState: Update.StateDownloaded,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "file:///usr/share/icons/suru/apps/sources/online-accounts-twitter.svg"
                        },
                        {
                            name: "StateFailed",
                            kind: Update.KindClick,
                            updateState: Update.StateFailed,
                            size: 10000000,
                            progress: 50,
                            version: "1.0",
                            changelog: "Changes",
                            iconUrl: "file:///usr/share/icons/suru/apps/sources/online-accounts-twitter.svg",
                            error: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec varius posuere odio, vitae sollicitudin libero. In at libero tempor, ultricies leo vel, porttitor quam. Nullam pharetra, ex vitae volutpat consectetur, neque ex vestibulum sem, ut condimentum ex orci in nunc."
                        }
                    ]
                }
            ]
        }

        function test_all(data) {
            var objs = [];
            for (var i = 0; i < data.datas.length; i++) {
                objs.push(update.createObject(col, data.datas[i]));
            }

            wait(15000);

            for (var i = 0; i < objs.length; i++) {
                objs[i].destroy();
            }
        }
    }
}
