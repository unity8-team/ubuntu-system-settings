/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
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
 */

#include <QDebug>
#include <QDBusReply>

#include "networkabout.h"

NetworkAbout::NetworkAbout(QObject *parent)
    : QObject(parent),
    m_systemBusConnection(QDBusConnection::systemBus())
{

    setupBluetoothMacAddress();
}

void NetworkAbout::setupBluetoothMacAddress()
{
    QDBusInterface bluezManagerInterface (
        "org.bluez",
        "/",
        "org.bluez.Manager",
        m_systemBusConnection,
        this);

    if (!bluezManagerInterface.isValid()) {
        return;
    }

    QDBusReply<QDBusObjectPath> defaultDevice = bluezManagerInterface.call("DefaultAdapter");

    if (!defaultDevice.isValid()) {
        return;
    }

    QDBusInterface adapterInterface (
        "org.bluez",
        defaultDevice.value().path(),
        "org.bluez.Adapter",
        m_systemBusConnection,
        this);

    QDBusReply<QVariantMap> props = adapterInterface.call("GetProperties");

    if (!props.isValid()) {
        return;
    }

    m_bluetoothMacAddress = props.value()["Address"].toString();
}

QString NetworkAbout::bluetoothMacAddress()
{
    return m_bluetoothMacAddress;
}
