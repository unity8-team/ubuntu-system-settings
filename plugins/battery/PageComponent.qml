/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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
import QMenuModel 0.1
import QtQuick 2.4
import QtSystemInfo 5.0
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.SystemSettings.Battery 1.0
import Ubuntu.SystemSettings.SecurityPrivacy 1.0
import Ubuntu.Settings.Components 0.1 as USC

ItemPage {
    id: root

    title: i18n.tr("Battery")
    flickable: scrollWidget

    function timeDeltaString(timeDelta) {
        var sec = timeDelta,
            min = Math.round (timeDelta / 60),
            hr = Math.round (timeDelta / 3600);
        if (sec < 60)
            // TRANSLATORS: %1 is the number of seconds
            return i18n.tr("%1 second ago", "%1 seconds ago", sec).arg(sec)
        else if (min < 60)
            // TRANSLATORS: %1 is the number of minutes
            return i18n.tr("%1 minute ago", "%1 minutes ago", min).arg(min)
        else
            // TRANSLATORS: %1 is the number of hours
            return i18n.tr("%1 hour ago", "%1 hours ago", hr).arg(hr)
    }

    GSettings {
        id: powerSettings
        schema.id: batteryBackend.powerdRunning ? "com.ubuntu.touch.system" : "org.gnome.desktop.session"
    }

    UbuntuSecurityPrivacyPanel {
        id: securityPrivacy
    }

    QDBusActionGroup {
        id: indicatorPower
        busType: 1
        busName: "com.canonical.indicator.power"
        objectPath: "/com/canonical/indicator/power"
        property variant brightness: action("brightness").state
        property variant batteryLevel: action("battery-level").state
        property variant deviceState: action("device-state").state
        Component.onCompleted: start()
    }

    UbuntuBatteryPanel {
        id: batteryBackend
    }

    /* Refresh the graph on a minute basis */
    Timer {
        interval: 60000; running: true; repeat: true
        onTriggered: canvas.requestPaint()
    }

    Connections {
        target: Qt.application
        onActiveChanged: {
            if (Qt.application.state === Qt.ApplicationActive) {
                canvas.requestPaint()
            }
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

            SettingsListItems.SingleValue {
                id: chargingLevel
                text: i18n.tr("Charge level")
                value: {
                    var chargeLevel = indicatorPower.batteryLevel

                    if (chargeLevel === undefined)
                        return i18n.tr("N/A")

                    /* TRANSLATORS: %1 refers to a percentage that indicates the charging level of the battery */
                    return i18n.tr("%1%".arg(chargeLevel))
                }

                showDivider: false
            }

            Canvas {
                id: canvas
                width:parent.width - units.gu(4)
                anchors.horizontalCenter: parent.horizontalCenter
                height: units.gu(23)

                /* Setting that property makes text not correct aliased for
                   some reasons, which happens with the value being false or
                   true, toolkit bug? see https://launchpad.net/bugs/1354363
                antialiasing: true */

                function drawAxes(ctx, axisWidth, axisHeight, bottomMargin, rightMargin) {

                    var currentHour = Qt.formatDateTime(new Date(), "h")
                    var currentMinutes = Qt.formatDateTime(new Date(), "m")
                    var displayHour
                    var labelWidth
                    var zeroMark

                    ctx.save()
                    ctx.beginPath()
                    ctx.strokeStyle = UbuntuColors.lightAubergine

                    ctx.lineWidth = units.dp(2)

                    var fontHeight = FontUtils.sizeToPixels("small")
                    ctx.font="%1px Ubuntu".arg(fontHeight)

                    ctx.translate(0, 1)

                    // 11 ticks with 0, 5, 10 being big
                    for (var i = 0; i <= 10; i++) {
                        var x = (i % 5 == 0) ? 0 : Math.floor(axisWidth / 2)
                        var y = (i / 10) * (height - axisHeight - bottomMargin - ctx.lineWidth)
                        ctx.moveTo(x, y)
                        ctx.lineTo(axisWidth, y)
                    }

                    ctx.translate(axisWidth + ctx.lineWidth / 2,
                                  height - axisHeight - bottomMargin - ctx.lineWidth / 2)

                    ctx.moveTo(0, 0)
                    ctx.lineTo(0, -ctx.lineWidth)

                    // 24 ticks with 6, 12, 18, 24 being big
                    for (i = 0; i <= 24; i++) {
                        /* the marks need to be shifted on the hours */
                        x = ((i - currentMinutes / 60) / 24) * (width - axisWidth - ctx.lineWidth - rightMargin)
                        if (x < 0)
                            continue
                        y = (i % 6 == 0) ? axisHeight : axisHeight -
                                            Math.floor(axisHeight / 2)
                        ctx.moveTo(x, 0)
                        ctx.lineTo(x, y)

                        /* Determine the hour to display */
                        displayHour = (currentHour - (24-i))
                        if (displayHour < 0)
                            displayHour = displayHour + 24
                        /* Store the x for the day change line */
                        if (displayHour === 0)
                            zeroMark = x

                        /* Write the x-axis legend */
                        if (i % 6 == 0) {
                            labelWidth = context.measureText("%1".arg(displayHour)).width;
                            ctx.fillText("%1".arg(displayHour),
                                         x - labelWidth/2,
                                         axisHeight + units.dp(1) + fontHeight)
                        }
                    }

                    labelWidth = context.measureText(i18n.tr("Yesterday")).width;
                    if(labelWidth < zeroMark)
                        ctx.fillText(i18n.tr("Yesterday"),
                                     (zeroMark - labelWidth)/2,
                                     axisHeight + units.dp(6) + 2*fontHeight)

                    ctx.fillText("|", zeroMark, axisHeight + units.dp(6) + 2*fontHeight)

                    labelWidth = context.measureText(i18n.tr("Today")).width;
                    if(labelWidth < (width - zeroMark - rightMargin - axisWidth - ctx.lineWidth))
                        ctx.fillText(i18n.tr("Today"),
                                     zeroMark + (width - zeroMark - labelWidth)/2,
                                     axisHeight + units.dp(6) + 2*fontHeight)

                    ctx.stroke()
                    ctx.restore()
                }

                onPaint:{
                    var ctx = canvas.getContext('2d');
                    ctx.save();

                    /* Use reset rather than clearRect due to QTBUG-36761 */
                    ctx.reset(0, 0, canvas.width, canvas.height)

                    var axisWidth = units.gu(1)
                    var axisHeight = units.gu(1)

                    /* Space to write the legend */
                    var bottomMargin = units.gu(6)
                    var rightMargin = units.gu(1)

                    drawAxes(ctx, axisWidth, axisHeight, bottomMargin, rightMargin)

                    /* Display the charge history */
                    ctx.beginPath();

                    ctx.lineWidth = units.dp(2)

                    /* Needed to avoid rendering glitches with point with the same x value
                       (#1461624/QTBUG-34339) */
                    ctx.lineJoin = "round"

                    ctx.translate(0, height)
                    // Invert the y axis so we draw from the bottom left
                    ctx.scale(1, -1)
                    // Move the origin to just above the axes
                    ctx.translate(axisWidth, axisHeight + bottomMargin)
                    // Scale to avoid the axes so we can draw as if they aren't
                    // there
                    ctx.scale(1 - ((axisWidth + rightMargin) / width),
                              1 - (axisHeight + bottomMargin) / height)

                    var gradient = ctx.createLinearGradient(0, 0, 0, height);
                    gradient.addColorStop(1, "green");
                    gradient.addColorStop(0.5, "yellow");
                    gradient.addColorStop(0, "red");
                    ctx.strokeStyle = gradient

                    /* Get infos from battery0, on a day (60*24*24=86400 seconds), with 150 points on the graph */
                    var chargeDatas = batteryBackend.getHistory(batteryBackend.deviceString, 86400, 150)

                    /* time is the offset in seconds compared to the current time (negative value)
                       we display the charge on a day, which is 86400 seconds, the value is the % */
                    ctx.moveTo((86400 - chargeDatas[0].time) / 86400 * width,
                               (chargeDatas[0].value / 100) * height)
                    for (var i = 1; i < chargeDatas.length; i++) {
                        ctx.lineTo((86400-chargeDatas[i].time) / 86400 * width,
                                   (chargeDatas[i].value / 100) * height)
                    }
                    ctx.stroke()
                    ctx.restore();
                }
            }

            SettingsListItems.SingleValue {
                id: chargingEntry
                text: {
                    if (indicatorPower.deviceState === "charging")
                        return i18n.tr("Charging now")
                    else if (indicatorPower.deviceState === "discharging")
                        return i18n.tr("Last full charge")
                    else if (indicatorPower.deviceState === "fully-charged")
                        return i18n.tr("Fully charged")
                    else
                        return ""
                }

                value: {
                    if (indicatorPower.deviceState === "discharging") {
                        if (batteryBackend.lastFullCharge)
                            return timeDeltaString(batteryBackend.lastFullCharge)
                        else
                            return i18n.tr("N/A")
                    }
                    else
                        return ""
                }
                showDivider: false
            }

            SettingsItemTitle {
                text: i18n.tr("Ways to reduce battery use:")
            }

            SettingsListItems.StandardProgression {
                text: i18n.tr("Display brightness")
                onClicked: {
                    var brightnessPlugin = pluginManager.getByName("brightness");
                    if (brightnessPlugin) {
                        pageStack.addPageToNextColumn(root,
                            brightnessPlugin.pageComponent, {
                                plugin: brightnessPlugin, pluginManager: pluginManager});
                    } else {
                        console.warn("Failed to get brightness plugin instance");
                    }
                }
            }

            SettingsListItems.SingleValueProgression {
                property bool lockOnSuspend:
                    securityPrivacy.securityType !==
                        UbuntuSecurityPrivacyPanel.Swipe
                text: lockOnSuspend ? i18n.tr("Lock when idle") : i18n.tr("Sleep when idle")
                value: {
                    if (batteryBackend.powerdRunning ) {
                        var timeout = powerSettings.activityTimeout
                        return timeout == 0 ?
                                    i18n.tr("Never") :
                                    (timeout < 60) ?
                                    // TRANSLATORS: %1 is the number of seconds
                                    i18n.tr("After %1 second",
                                            "After %1 seconds",
                                             timeout).arg(timeout) :
                                    // TRANSLATORS: %1 is the number of minutes
                                    i18n.tr("After %1 minute",
                                            "After %1 minutes",
                                            Math.round(timeout/60)).arg(Math.round(timeout/60))
                    }
                    else {
                        var timeout = Math.round(powerSettings.idleDelay/60)
                        return (powerSettings.idleDelay != 0) ?
                                    // TRANSLATORS: %1 is the number of minutes
                                    i18n.tr("After %1 minute",
                                            "After %1 minutes",
                                            timeout).arg(timeout) :
                                    i18n.tr("Never")
                    }
                }

                Icon {
                    width: units.gu(2.5)
                    height: width
                    name: "network-secure"
                    SlotsLayout.position: SlotsLayout.First
                }

                onClicked: pageStack.addPageToNextColumn(root,
                    Qt.resolvedUrl("SleepValues.qml"),{
                        title: text, lockOnSuspend: lockOnSuspend })
                visible: !isSnap || showAllUI
            }

            QDBusActionGroup {
                id: networkActionGroup
                busType: 1
                busName: "com.canonical.indicator.network"
                objectPath: "/com/canonical/indicator/network"
                property variant enabled: action("wifi.enable")
                Component.onCompleted: start()
            }

            SettingsListItems.Icon {
                // TRANSLATORS: “Wi-Fi used for hotspot” is hidden.
                text: showAllUI ? i18n.tr("Wi-Fi used for hotspot") : i18n.tr("Wi-Fi")
                iconName: "wifi-high"

                Loader {
                    active: networkActionGroup.enabled.state != null
                    sourceComponent: Switch {
                        id: wifiSwitch
                        property bool serverChecked: networkActionGroup.enabled.state

                        USC.ServerPropertySynchroniser {
                            userTarget: wifiSwitch
                            userProperty: "checked"
                            serverTarget: wifiSwitch
                            serverProperty: "serverChecked"

                            onSyncTriggered: networkActionGroup.enabled.activate()
                        }
                    }
                }
                visible: networkActionGroup.enabled.state !== undefined
            }

            QDBusActionGroup {
                id: bluetoothActionGroup
                busType: DBus.SessionBus
                busName: "com.canonical.indicator.bluetooth"
                objectPath: "/com/canonical/indicator/bluetooth"

                property bool visible: action("bluetooth-supported")
                property variant enabled: action("bluetooth-enabled")

                Component.onCompleted: start()
            }

            SettingsListItems.Icon {
                id: btListItem
                text: i18n.tr("Bluetooth")
                iconName: "bluetooth-active"

                Loader {
                    active: bluetoothActionGroup.enabled.state != null
                    sourceComponent: Switch {
                        id: btSwitch
                        property bool serverChecked: bluetoothActionGroup.enabled.state

                        USC.ServerPropertySynchroniser {
                            userTarget: btSwitch
                            userProperty: "checked"
                            serverTarget: btSwitch
                            serverProperty: "serverChecked"

                            onSyncTriggered: bluetoothActionGroup.enabled.activate()
                        }
                    }
                }
                visible: bluetoothActionGroup.visible
            }
        }
    }
}
