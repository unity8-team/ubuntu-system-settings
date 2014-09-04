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

#include "bluetooth.h"

#include <QQmlEngine>

#include "agent.h"
#include "agentadaptor.h"
#include "dbus-shared.h"

Bluetooth::Bluetooth(QObject *parent):
    Bluetooth(QDBusConnection::systemBus(), parent)
{
}

Bluetooth::Bluetooth(const QDBusConnection &dbus, QObject *parent):
    QObject(parent),
    m_dbus(dbus),
    m_devices(m_dbus),
    m_agent(m_dbus, m_devices)
{
    // export our Agent to handle pairing requests
    new AgentAdaptor(&m_agent);
    if(!m_dbus.registerObject(DBUS_AGENT_PATH, &m_agent))
        qCritical() << "Couldn't register agent at" << DBUS_AGENT_PATH;

    m_connectedDevices.filterOnConnections(Device::Connection::Connected |
                                           Device::Connection::Connecting |
                                           Device::Connection::Disconnecting);
    m_connectedDevices.setSourceModel(&m_devices);

    m_disconnectedDevices.filterOnConnections(Device::Connection::Disconnected);
    m_disconnectedDevices.filterOnTrusted(false);
    m_disconnectedDevices.setSourceModel(&m_devices);

    m_autoconnectDevices.filterOnConnections(Device::Connection::Disconnected);
    m_autoconnectDevices.filterOnTrusted(true);
    m_autoconnectDevices.setSourceModel(&m_devices);

    QObject::connect(&m_devices, SIGNAL(poweredChanged(bool)),
                     this, SIGNAL(poweredChanged(bool)));

    QObject::connect(&m_devices, SIGNAL(discoveringChanged(bool)),
                     this, SIGNAL(discoveringChanged(bool)));

    QObject::connect(&m_devices, SIGNAL(discoverableChanged(bool)),
                     this, SIGNAL(discoverableChanged(bool)));
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

bool Bluetooth::isSupportedType(const int type)
{
    switch((Device::Type)type) {

    case Device::Type::Headset:
    case Device::Type::Headphones:
    case Device::Type::OtherAudio:
        return true;

    default:
        return false;
    }
}

/***
****
***/

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

QAbstractItemModel * Bluetooth::getAutoconnectDevices()
{
    auto ret = &m_autoconnectDevices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}


/***
****
***/

void Bluetooth::disconnectDevice()
{
    if (m_selectedDevice) {
        auto type = m_selectedDevice->getType();
        if (type == Device::Type::Headset)
            m_selectedDevice->disconnect(Device::ConnectionMode::Audio);
        else if (type == Device::Type::Headphones)
            m_selectedDevice->disconnect(Device::ConnectionMode::Audio);
        else if (type == Device::Type::OtherAudio)
            m_selectedDevice->disconnect(Device::ConnectionMode::Audio);
    } else {
        qWarning() << "No selected device to disconnect";
    }
}

void Bluetooth::connectDevice(const QString &address)
{
    Device::ConnectionMode connMode;
    auto device = m_devices.getDeviceFromAddress(address);
    Device::Type type;

    if (!device) {
        qWarning() << "No device to connect.";
        return;
    }

    type = device->getType();
    if (type == Device::Type::Headset)
        connMode = Device::ConnectionMode::Audio;
    else if (type == Device::Type::Headphones)
        connMode = Device::ConnectionMode::Audio;
    else if (type == Device::Type::OtherAudio)
        connMode = Device::ConnectionMode::Audio;

    if (device->isTrusted()) {
        device->connect(connMode);
    } else {
        m_devices.addConnectAfterPairing(address, connMode);
        m_devices.createDevice(address);
    }
}

void Bluetooth::removeDevice()
{
    if (m_selectedDevice) {
        QString path = m_selectedDevice->getPath();
        m_devices.removeDevice(path);
    } else {
        qWarning() << "No selected device to remove.";
    }
}

