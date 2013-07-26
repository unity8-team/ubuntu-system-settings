/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Didier Roche <didier.roches@canonical.com>
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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Update 1.0


ItemPage {
    id: root

    title: i18n.tr("Update phone")
    flickable: scrollWidget // maybe remove

    UbuntuUpdatePanel {
        id: updateID

        property bool updateInProgress: false
        property bool updateReady: false
        property bool updateCanceled: false
        property bool updateFailed: false

        function startUpdate() {
            updateInProgress= true;
            updateReady = false;
            updateCanceled = false;
            updateFailed = false;
            actionbuttons.text = ""
            TriggerUpdate();
        }

        onReadyToReboot: {
            updateInProgress = true;
            updateReady = true;
            updateCanceled = false;
            updateFailed = false;
        }
        onUpdateFailed: {
            updateInProgress = false;
            updateReady = false;
            updateCanceled = false;
            updateFailed = true;
            actionbuttons.text = i18n.tr("Sorry, the update failed. Please try again later");
        }
        onUpdateCanceled: {
            updateInProgress = false;
            updateReady = false;
            updateCanceled = true;
            updateFailed = false;
            statusDetails.opacity = 0.0;
        }

    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Standard {
                id: toto
                text: { if (updateID.updateAvailable === 0)
                          return i18n.tr("Congrats! You are already up to date!");
                        else if (updateID.updateAvailable === 1)
                          return i18n.tr("New version is available, click for more details");
                        return i18n.tr("Checking latest available system version…"); }
                onClicked: { if (updateID.updateAvailable === 1)
                               statusDetails.opacity = 1.0;
                           }
            }
            ListItem.Standard {
                id: statusDetails
                Behavior on opacity { PropertyAnimation { duration: 1000 } }
                opacity: 0
                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    ListItem.Standard {
                        text: i18n.tr("You can update from version %1 to version %2".arg(updateID.OSVersion).arg(updateID.updateVersion));
                    }
                    ListItem.Standard {
                        text: i18n.tr("Size of this update: %1".arg(updateID.updateSize));
                    }

                    ListItem.Standard {
                        // TODO fix with real description (probably iteraring on a list)
                        // no i18n as the chain will change
                        text: "This update will:\n * Enables a 200x improvment on Ubuntu Edge phone\n * Makes you a sandwish";
                    }
                    ListItem.Standard {
                        id: actionbuttons
                        text: "Apply?"
                        ActivityIndicator {
                            anchors.verticalCenter: parent.verticalCenter
                            running: updateID.updateInProgress && !updateID.updateReady
                        }
                        control: Row {
                            spacing: units.gu(1)
                            Button {
                                text: i18n.tr("Update your phone")
                                width: units.gu(19)
                                onClicked: updateID.startUpdate()
                                visible: !updateID.updateInProgress && !updateID.updateFailed
                            }
                            Button {
                                text: i18n.tr("Cancel update")
                                width: units.gu(19)
                                onClicked: updateID.CancelUpdate()
                                visible: updateID.updateInProgress
                            }
                            Button {
                                text: i18n.tr("Reboot your phone")
                                width: units.gu(19)
                                onClicked: updateID.Reboot()
                                visible: updateID.updateInProgress && updateID.updateReady
                            }
                        }
                    }
                }
            }
        }
    }

}
