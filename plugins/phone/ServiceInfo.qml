/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Sebastien Bacher <sebastien.bacher@canonical.com>
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
import Ubuntu.History 0.1
import "dateUtils.js" as DateUtils

ItemPage {
    property string serviceName
    property string serviceNumber
    title: serviceName            

    HistoryEventModel {
        id: historyEventModel
        type: HistoryThreadModel.EventTypeVoice
        sort: HistorySort {
            sortField: "timestamp"
            sortOrder: HistorySort.DescendingOrder
        }

        property string phoneNumber: serviceNumber

        filter: HistoryUnionFilter {
            // FIXME: this is not the best API for this case, but will be changed later
            HistoryIntersectionFilter {
                HistoryFilter {
                    property string threadId: historyEventModel.threadIdForParticipants("ofono/ofono/account0",
                                                                                        HistoryThreadModel.EventTypeVoice,
                                                                                        [historyEventModel.phoneNumber],
                                                                                        HistoryThreadModel.MatchPhoneNumber);
                    filterProperty: "threadId"
                    filterValue: threadId != "" ? threadId : "something that won't match"
                }
                HistoryFilter {
                    filterProperty: "accountId"
                    filterValue: "ofono/ofono/account0"
                }
            }

            HistoryIntersectionFilter {
                HistoryFilter {
                    property string threadId: historyEventModel.threadIdForParticipants("ofono/ofono/account1",
                                                                                        HistoryThreadModel.EventTypeVoice,
                                                                                        [historyEventModel.phoneNumber],
                                                                                        HistoryThreadModel.MatchPhoneNumber);
                    filterProperty: "threadId"
                    filterValue: threadId != "" ? threadId : "something that won't match"
                }
                HistoryFilter {
                    filterProperty: "accountId"
                    filterValue: "ofono/ofono/account1"
                }
            }
        }
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

        Repeater {
            id: lastCalled
            anchors.left: parent.left
            anchors.right: parent.right
            //anchors.fill: parent
            //height: childrenRect.height
            model: historyEventModel
            delegate: ListItem.Base {
                anchors.left: parent.left
                anchors.right: parent.right
                height: lastCalledCol.height + units.gu(6)
                visible: index === 0
                Column {
                    id: lastCalledCol
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: childrenRect.height
                    spacing: units.gu(2)

                    UbuntuShape {
                        width: parent.width / 2
                        height: width
                        anchors.horizontalCenter: parent.horizontalCenter
                        image: Icon {
                            anchors.fill: parent
                            name: "contact"
                        }
                    }

                    Label {
                        id: calledLabel
                        objectName: "calledLabel"
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: i18n.tr("Last called %1").arg(DateUtils.formatFriendlyDate(timestamp))
                    }
                }
            }
        }
    }

    ListItem.SingleControl {
        anchors.bottom: parent.bottom
        control: Button {
            width: parent.width - units.gu(4)
            text: i18n.tr("Call")
            onClicked: Qt.openUrlExternally("tel:///" + serviceNumber)
        }
    }
}
