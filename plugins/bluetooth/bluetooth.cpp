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

#include <QDebug>
#include <QQmlEngine>

#include "agent.h"
#include "agentadaptor.h"
#include "bluetooth.h"
#include "dbus-shared.h"

Bluetooth :: Bluetooth (QObject * parent):
  QObject (parent),
  m_dbus (QDBusConnection::systemBus()),
  m_devices (m_dbus),
  m_agent (m_dbus, m_devices)
{
  // export our Agent to handle pairing requests
  new AgentAdaptor (&m_agent);
  if (!m_dbus.registerObject (DBUS_AGENT_PATH, &m_agent))
    qFatal ("Couldn't register agent at " DBUS_AGENT_PATH);

  m_connectedHeadsets.filterOnType (Device::Type::Headset);
  m_connectedHeadsets.filterOnConnections (Device::Connection::Connected | Device::Connection::Disconnecting);
  m_connectedHeadsets.setSourceModel (&m_devices);

  m_disconnectedHeadsets.filterOnType (Device::Type::Headset);
  m_disconnectedHeadsets.filterOnConnections (Device::Connection::Connecting | Device::Connection::Disconnected);
  m_disconnectedHeadsets.setSourceModel (&m_devices);
  
  QObject::connect (&m_killswitch, SIGNAL(blockedChanged(bool)),
                    this, SLOT(onKillSwitchChanged(bool)));

  QObject::connect (&m_agent, SIGNAL(onPairingDone()),
                    this, SLOT(onPairingDone()));
}

void
Bluetooth :: onKillSwitchChanged (bool blocked)
{
  Q_EMIT (enabledChanged (!blocked));
}

void
Bluetooth :: setSelectedDevice (const QString& address)
{
  if (!m_selectedDevice || (m_selectedDevice->getAddress() != address))
    {
      m_selectedDevice = m_devices.getDeviceFromAddress (address);
      Q_EMIT (selectedDeviceChanged ());
    }
}

/***
****
***/

Device *
Bluetooth :: getSelectedDevice ()
{
  if (m_selectedDevice)
    {
      auto ret = m_selectedDevice.data();
      QQmlEngine::setObjectOwnership (ret, QQmlEngine::CppOwnership);
      return ret;
    }

  return nullptr;
}

Agent *
Bluetooth :: getAgent ()
{
  auto ret = &m_agent;
  QQmlEngine::setObjectOwnership (ret, QQmlEngine::CppOwnership);
  return ret;
}

QAbstractItemModel *
Bluetooth :: getConnectedHeadsets ()
{
  auto ret = &m_connectedHeadsets;
  QQmlEngine::setObjectOwnership (ret, QQmlEngine::CppOwnership);
  return ret;
}

QAbstractItemModel *
Bluetooth :: getDisconnectedHeadsets ()
{
  auto ret = &m_disconnectedHeadsets;
  QQmlEngine::setObjectOwnership (ret, QQmlEngine::CppOwnership);
  return ret;
}


/***
****
***/

void
Bluetooth :: disconnectHeadset ()
{
  if (m_selectedDevice)
    m_selectedDevice->disconnect (Device::HeadsetMode);
}

void
Bluetooth :: connectHeadset (const QString& address)
{
  const Device::ConnectionMode connMode = Device::HeadsetMode;
  auto device = m_devices.getDeviceFromAddress (address);
  if (!device)
    return;

  if (device->isPaired())
    {
      device->connect (connMode);
    }
  else
    {
      m_connectAfterPairing[address] = connMode;
      m_devices.pairDevice (address);
    }
}

void
Bluetooth :: onPairingDone ()
{
  QMapIterator<QString,Device::ConnectionMode> it (m_connectAfterPairing);

  while (it.hasNext())
    {
      it.next();
      const QString& address = it.key();
      auto device = m_devices.getDeviceFromAddress (address);
      if (device)
        {
          device->connect (it.value());
        }
    }

  m_connectAfterPairing.clear();
}
