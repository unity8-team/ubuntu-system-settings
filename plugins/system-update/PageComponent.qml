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
import Ubuntu.Components.Popups 0.1
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

        // TODO: example data, no i18n
        property variant updateDescriptions: ["Enables a 200x improvment on Ubuntu Edge phone",
                                              "Makes you a sandwich",
                                              "Makes Steve and Loic happy"]

        function startUpdate() {
            updateInProgress= true;
            updateReady = false;
            updateCanceled = false;
            actionbuttons.text = "";
            TriggerUpdate();
        }

        onReadyToReboot: {
            updateInProgress = true;
            updateReady = true;
            updateCanceled = false;
        }
        onUpdateFailed: {
            PopupUtils.open(updateFailedDialog)
            updateInProgress = false;
            updateReady = false;
            updateCanceled = false;
            updateAvailable = -1;
        }
        onUpdateCanceled: {
            updateInProgress = false;
            updateReady = false;
            updateCanceled = true;
            updateAvailable = 1;
        }

        onUpdateAvailableChanged: {
            if (updateID.updateAvailable === 1) {
                statusDetails.opacity = 1.0;
                actionbuttons.text = actionbuttons.default_text;
            }
            else
                statusDetails.opacity = 0.0;
        }

    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        anchors.left: parent.left
        anchors.right: parent.right


        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Base {
                height: updateStatusbar.height + checkUpdateIndicator.height + units.gu(6)

                Column {
                    width: parent.width
                    anchors.centerIn: parent
                    spacing: units.gu(2)

                    Label {
                        id: updateStatusbar
                        anchors.left: parent.left
                        anchors.right: parent.right

                        fontSize: "large"
                        text: { if (updateID.updateAvailable === 0)
                                  return i18n.tr("Congrats! You are already up to date!");
                                else if (updateID.updateAvailable === 1)
                                  return i18n.tr("A new version is available!");
                                return i18n.tr("Checking latest available system version…"); }
                        wrapMode: Text.WordWrap

                    }
                    ActivityIndicator {
                        id: checkUpdateIndicator
                        anchors.horizontalCenter: parent.horizontalCenter
                        running: updateID.updateAvailable < 0
                        visible: running
                    }
                    ProgressBar {
                        id: indeterminateBar
                        anchors.left: parent.left
                        anchors.right: parent.right

                        indeterminate: true
                        visible: updateID.updateInProgress && !updateID.updateReady
                    }
                }

            }

            ListItem.Standard {
                id: statusDetails
                Behavior on opacity { PropertyAnimation { duration: 1000 } }
                opacity: 0

                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    // FIXME: Any of those item is creating an extra line in the middle, commented for now
                    //ListItem.Divider { }
                    /*ListItem.Header {
                        text: i18n.tr("General update infos:")
                    }*/
                    ListItem.Standard {
                        text: i18n.tr("You can update from version %1 to version %2").arg(updateID.OSVersion).arg(updateID.updateVersion);
                    }
                    ListItem.Standard {
                        text: i18n.tr("Size of this update: %1").arg(updateID.updateSize);
                    }
                    ListItem.Divider { }
                    ListItem.Header {
                        text: i18n.tr("This update will:")
                    }
                    Repeater {
                        model: updateID.updateDescriptions
                        ListItem.Standard { text: modelData }
                    }
                    ListItem.Divider { }

                    ListItem.Standard {
                        id: actionbuttons
                        property string default_text: i18n.tr("Apply?")
                        text: default_text
                        control: Row {
                            spacing: units.gu(1)
                            Button {
                                text: i18n.tr("Update your phone")
                                width: units.gu(19)
                                onClicked: updateID.startUpdate()
                                visible: !updateID.updateInProgress
                            }
                            Button {
                                text: i18n.tr("Cancel update")
                                width: units.gu(19)
                                onClicked: updateID.CancelUpdate()
                                visible: updateID.updateInProgress
                            }
                            Button {
                                text: i18n.tr("Reboot your phone now")
                                width: units.gu(25)
                                onClicked: updateID.Reboot()
                                visible: updateID.updateInProgress && updateID.updateReady
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
         id: updateFailedDialog
        Dialog {
            id: updateFailedDialogue
            title: i18n.tr("We are deeply sorry");
            text: i18n.tr("The update failed. Please try again later");
            Button {
               text: "I'll try later"
               onClicked: PopupUtils.close(updateFailedDialogue)
            }
        }
    }
}
