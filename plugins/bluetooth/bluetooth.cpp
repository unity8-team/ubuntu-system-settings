/*
 * Copyright (C) 2013-2015 Canonical Ltd
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
    new BluezAgent1Adaptor(&m_agent);
    if(!m_dbus.registerObject(DBUS_ADAPTER_AGENT_PATH, &m_agent))
        qCritical() << "Couldn't register agent at" << DBUS_ADAPTER_AGENT_PATH;

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

    QObject::connect(&m_devices, SIGNAL(devicePairingDone(Device*,bool)),
                     this, SIGNAL(devicePairingDone(Device*,bool)));
}

void Bluetooth::setSelectedDevice(const QString &address)
{
    if (!m_selectedDevice || (m_selectedDevice->getAddress() != address)) {
        m_selectedDevice = m_devices.getDeviceFromAddress(address);
        Q_EMIT(selectedDeviceChanged());
    }
}

void Bluetooth::resetSelectedDevice()
{
    m_selectedDevice.reset(0);
    Q_EMIT(selectedDeviceChanged());
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

void Bluetooth::trySetDiscoverable(bool discoverable)
{
    m_devices.trySetDiscoverable(discoverable);
}

bool Bluetooth::isSupportedType(const int type)
{
    switch((Device::Type)type) {

    case Device::Type::Headset:
    case Device::Type::Headphones:
    case Device::Type::Speakers:
    case Device::Type::Carkit:
    case Device::Type::OtherAudio:
    case Device::Type::Keyboard:
    case Device::Type::Mouse:
    case Device::Type::Tablet:
    case Device::Type::Watch:
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
    if (!m_selectedDevice)
        return;

    m_selectedDevice->disconnect();
}

void Bluetooth::connectDevice(const QString &address)
{
    auto device = m_devices.getDeviceFromAddress(address);

    if (!device) {
        qWarning() << "No device to connect.";
        return;
    }

    if (!device->isPaired()) {
        device->setConnectAfterPairing(true);
        device->pair();
    }
    else {
        device->connect();
    }
}

void Bluetooth::removeDevice()
{
    if (!m_selectedDevice) {
        qWarning() << "No selected device to remove.";
        return;
    }

    QString path = m_selectedDevice->getPath();
    m_devices.removeDevice(path);
}

