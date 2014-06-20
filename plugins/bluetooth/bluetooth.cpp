/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Charles Kerr <charles.kerr@canonical.com>
 *
 */

#include <QQmlEngine>

#include "agent.h"
#include "agentadaptor.h"
#include "bluetooth.h"
#include "dbus-shared.h"

Bluetooth::Bluetooth(QObject *parent):
    QObject(parent),
    m_dbus(QDBusConnection::systemBus()),
    m_devices(m_dbus),
    m_agent(m_dbus, m_devices)
{
    // export our Agent to handle pairing requests
    new AgentAdaptor(&m_agent);
    if(!m_dbus.registerObject(DBUS_AGENT_PATH, &m_agent))
        qFatal("Couldn't register agent at " DBUS_AGENT_PATH);

    QVector<Device::Type> types;
    types.append(Device::Type::Headset);
    types.append(Device::Type::Headphones);
    types.append(Device::Type::OtherAudio);
    m_connectedDevices.filterOnType(types);
    m_connectedDevices.filterOnConnections(Device::Connection::Connected |
                                           Device::Connection::Disconnecting);
    m_connectedDevices.setSourceModel(&m_devices);

    m_disconnectedDevices.filterOnType(types);
    m_disconnectedDevices.filterOnConnections(Device::Connection::Connecting |
                                              Device::Connection::Disconnected);
    m_disconnectedDevices.setSourceModel(&m_devices);

    QObject::connect(&m_devices, SIGNAL(discoveringChanged(bool)),
                     this, SIGNAL(discoveringChanged(bool)));

    QObject::connect(&m_devices, SIGNAL(discoverableChanged(bool)),
                     this, SIGNAL(discoverableChanged(bool)));

    QObject::connect(&m_agent, SIGNAL(onPairingDone()),
                     this, SLOT(onPairingDone()));
}

void Bluetooth::setSelectedDevice(const QString &address)
{
    if (!m_selectedDevice || (m_selectedDevice->getAddress() != address)) {
        m_selectedDevice = m_devices.getDeviceFromAddress(address);
        Q_EMIT(selectedDeviceChanged());
    }
}

void Bluetooth::toggleDiscovery()
{
    m_devices.toggleDiscovery();
}

void Bluetooth::startDiscovery()
{
    m_devices.startDiscovery();
}

void Bluetooth::stopDiscovery()
{
    m_devices.stopDiscovery();
}

/***
****
***/

QString Bluetooth::getAdapterName()
{
    return m_devices.getAdapterName();
}

Device * Bluetooth::getSelectedDevice()
{
    if (m_selectedDevice) {
        auto ret = m_selectedDevice.data();
        QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
        return ret;
    }

    return nullptr;
}

Agent * Bluetooth::getAgent()
{
    auto ret = &m_agent;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

QAbstractItemModel * Bluetooth::getConnectedDevices()
{
    auto ret = &m_connectedDevices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

QAbstractItemModel * Bluetooth::getDisconnectedDevices()
{
    auto ret = &m_disconnectedDevices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}


/***
****
***/

void Bluetooth::disconnectDevice()
{
    Device::Type type;

    if (m_selectedDevice)
        type = m_selectedDevice->getType();
        if (type == Device::Type::Headset)
            m_selectedDevice->disconnect(Device::ConnectionMode::Audio);
        else if (type == Device::Type::Headphones)
            m_selectedDevice->disconnect(Device::ConnectionMode::Audio);
        else if (type == Device::Type::OtherAudio)
            m_selectedDevice->disconnect(Device::ConnectionMode::Audio);
}

void Bluetooth::connectDevice(const QString &address)
{
    Device::ConnectionMode connMode;
    auto device = m_devices.getDeviceFromAddress(address);
    Device::Type type;

    if (!device)
        return;

    type = device->getType();
    if (type == Device::Type::Headset)
        connMode = Device::ConnectionMode::Audio;
    else if (type == Device::Type::Headphones)
        connMode = Device::ConnectionMode::Audio;
    else if (type == Device::Type::OtherAudio)
        connMode = Device::ConnectionMode::Audio;

    if (device->isPaired()) {
        device->connect(connMode);
    } else {
        m_connectAfterPairing[address] = connMode;
        m_devices.pairDevice(address);
    }
}

void Bluetooth::onPairingDone()
{
    QMapIterator<QString,Device::ConnectionMode> it(m_connectAfterPairing);
    while (it.hasNext()) {
        it.next();
        const QString &address = it.key();
        auto device = m_devices.getDeviceFromAddress(address);
        if (device)
            device->connect(it.value());
    }

    m_connectAfterPairing.clear();
}
