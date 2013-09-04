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

import GSettings 1.0
import QtQuick 2.0
import QtSystemInfo 5.0
import SystemSettings 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.SystemSettings.Battery 1.0

ItemPage {
    id: root

    title: i18n.tr("Battery")
    flickable: scrollWidget

    property bool isCharging

    function timeDeltaString(timeDelta) {
        if (timeDelta === 1)
            return i18n.tr("1 second ago")
        if (timeDelta < 60)
            return i18n.tr("%1 seconds ago").arg(timeDelta)
        if (timeDelta >= 60 && timeDelta < 90)
            return i18n.tr("1 minute ago")
        if (timeDelta >= 90 && timeDelta < 3570)
            return i18n.tr("%1 minutes ago").arg(Math.round(timeDelta/60))
        if (timeDelta >= 3570 && timeDelta < 5400)
            return i18n.tr("1 hour ago")
        if (timeDelta >= 5400 && timeDelta < 84600)
            return i18n.tr("%1 hours ago").arg(Math.round(timeDelta/3600))
        if (timeDelta >= 84600 && timeDelta < 129600)
            return i18n.tr("1 day ago")
        if (timeDelta >= 129600)
            return i18n.tr("%1 days ago").arg(Math.round(timeDelta/86400))
    }

    GSettings {
        id: powerSettings
        schema.id: batteryBackend.powerdRunning ? "com.canonical.powerd" : "org.gnome.settings-daemon.plugins.power"
    }

    GSettings {
        id: settingsSchema
        schema.id: "com.ubuntu.touch.system-settings"
    }

    BatteryInfo {
        id: batteryInfo

        monitorChargingState: true
        monitorBatteryStatus: true

        onChargingStateChanged: {
            if (state === BatteryInfo.Charging) {
                chargingEntry.text = i18n.tr("Charging now")
                isCharging = true
            }
            else if (state === BatteryInfo.Discharging) {
                chargingEntry.text = i18n.tr("Last full charge")
                isCharging = false
            }
            else if (state === BatteryInfo.Full || state === BatteryInfo.NotCharging) {
                chargingEntry.text = i18n.tr("Fully charged")
                isCharging = true
            }
        }
        onRemainingCapacityChanged: {
            if(batteryInfo.batteryCount > 0)
                chargingLevel.value = i18n.tr("%1 %").arg((batteryInfo.remainingCapacity(0)/batteryInfo.maximumCapacity(0)*100).toFixed(0))
        }

        Component.onCompleted: {
            onChargingStateChanged(0, chargingState(0))
            onRemainingCapacityChanged(0, remainingCapacity(0))
        }
    }

    UbuntuBatteryPanel {
        id: batteryBackend
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.SingleValue {
                id: chargingLevel
                text: i18n.tr("Charge level")
                value: i18n.tr("N/A")
                showDivider: false
            }

            ListItem.SingleValue {
                id: chargingEntry
                value: isCharging ?
                           "" : (batteryBackend.lastFullCharge ? timeDeltaString(batteryBackend.lastFullCharge) : i18n.tr("N/A"))
                showDivider: false
            }

            Canvas {
                id: canvas
                width:parent.width - units.gu(4)
                anchors.horizontalCenter: parent.horizontalCenter
                height: units.gu(15)

                onPaint:{
                    var ctx = canvas.getContext('2d');
                    ctx.save();
                    ctx.clearRect(0, 0, canvas.width, canvas.height)
                    ctx.beginPath();

                    /* Display the axis in aubergine color */
                    ctx.strokeStyle = UbuntuColors.lightAubergine
                    ctx.lineWidth = units.dp(3)
                    ctx.moveTo(1, 0)
                    ctx.lineTo(1, height)
                    ctx.lineTo(width, height)
                    ctx.stroke()

                    /* Display the charge history in orange color */
                    ctx.lineWidth = units.dp(2)
                    ctx.strokeStyle = UbuntuColors.orange

                    /* Get infos from battery0, on a day (60*24*24=86400 seconds), with 150 points on the graph */
                    var chargeDatas = batteryBackend.getHistory(batteryBackend.deviceString, 86400, 150)

                    /* time is the offset in seconds compared to the current time (negative value)
                       we display the charge on a day, which is 86400 seconds, the value is the %
                       the coordinates are adjusted to the canvas, top,left is 0,0 */
                    ctx.moveTo((86400-chargeDatas[0].time)/86400*canvas.width, (1-chargeDatas[0].value/100)*canvas.height)
                    for (var i=1; i < chargeDatas.length; i++) {
                        ctx.lineTo((86400-chargeDatas[i].time)/86400*canvas.width, (1-chargeDatas[i].value/100)*canvas.height)
                    }
                    ctx.stroke()
                    ctx.restore();
                }
            }

            ListItem.Standard {
                text: i18n.tr("Ways to reduce battery use:")
            }

            ListItem.Standard {
                text: i18n.tr("Display brightness")
                showDivider: false
            }

            ListItem.Base {
                    Icon {
                        id: iconLeft
                        anchors.verticalCenter: parent.verticalCenter
                        height: sliderId.height - units.gu(1)
                        name: "torch-off"
                        width: height
                    }
                    Slider {
                        id: sliderId
                        anchors {
                            left: iconLeft.right
                            right: iconRight.left
                            leftMargin: units.gu(1)
                            rightMargin: units.gu(1)
                            verticalCenter: parent.verticalCenter
                        }
                        height: parent.height - units.gu(2)
                        minimumValue: 0
                        maximumValue: 100
                        value: 0.0 // TODO: get actual value
                    }
                    Icon {
                        id: iconRight
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        height: sliderId.height - units.gu(1)
                        name: "torch-on"
                        width: height
                    }
            }

            ListItem.SingleValue {
                /* TODO: use real configuration when we have one, lp #1215957 */
                property bool lockOnSuspend:
                    settingsSchema.unlockMethod !== "swipe"
                text: lockOnSuspend ? i18n.tr("Lock when idle") : i18n.tr("Sleep when idle")
                value: {
                    if (batteryBackend.powerdRunning ) {
                        var timeout = Math.round(powerSettings.activityTimeout/60)
                        return (powerSettings.activityTimeout != 0) ?
                                    i18n.tr("After %1 minute".arg(timeout),
                                            "After %1 minutes".arg(timeout),
                                            timeout) :
                                    i18n.tr("Never")
                    }
                    else {
                        var timeout = Math.round(powerSettings.sleepDisplayBattery/60)
                        return (powerSettings.sleepDisplayBattery != 0) ?
                                    i18n.tr("After %1 minute".arg(timeout),
                                            "After %1 minutes".arg(timeout),
                                            timeout) :
                                    i18n.tr("Never")
                    }
                }
                progression: true
                onClicked: pageStack.push(
                               Qt.resolvedUrl("SleepValues.qml"),
                               { title: text, lockOnSuspend: lockOnSuspend })
            }
            ListItem.Standard {
                text: i18n.tr("Wi-Fi")
                control: Switch {
                    checked: batteryBackend.getWifiStatus()
                    onCheckedChanged: batteryBackend.setWifiStatus(checked)
                }
            }

            ListItem.Standard {
                text: i18n.tr("Bluetooth")
                control: Switch {
                    checked: true
                    enabled: false
                }
            }

            ListItem.Standard {
                text: i18n.tr("GPS")
                control: Switch {
                    checked: true
                    enabled: false
                }
            }

            ListItem.Caption {
                text: i18n.tr("Accurate location detection requires GPS and/or Wi-Fi.")
            }
        }
    }
}
