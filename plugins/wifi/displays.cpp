/*
 * Copyright (C) 2015 Canonical Ltd
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
 * Ken VanDine <ken.vandine@canonical.com>
 *
 */

#include "displays.h"
#include "aethercast_manager.h"
#include "aethercast_device.h"

#include <QQmlEngine>

Displays::Displays(QObject *parent):
    Displays(QDBusConnection::systemBus(), parent)
{
    qWarning() << Q_FUNC_INFO;
}

Displays::Displays(const QDBusConnection &dbus, QObject *parent):
    QObject(parent),
    m_dbus(dbus)
    //m_devices(m_dbus)
{
    qWarning() << Q_FUNC_INFO;
    /*
    if(!m_dbus.registerObject(DBUS_ADAPTER_AGENT_PATH, &m_agent))
        qCritical() << "Couldn't register agent at" << DBUS_ADAPTER_AGENT_PATH;

    m_connectedDevices.setSourceModel(&m_devices);

    m_disconnectedDevices.setSourceModel(&m_devices);

    QObject::connect(&m_devices, SIGNAL(discoveringChanged(bool)),
                     this, SIGNAL(discoveringChanged(bool)));
    */
}

/*
void Displays::toggleDiscovery()
{
    qWarning() << Q_FUNC_INFO;
    m_devices.toggleDiscovery();
}

void Displays::startDiscovery()
{
    qWarning() << Q_FUNC_INFO;
    m_devices.startDiscovery();
}

void Displays::stopDiscovery()
{
    qWarning() << Q_FUNC_INFO;
    m_devices.stopDiscovery();
}
*/

/***
****
***/

/*
QAbstractItemModel * Displays::getConnectedDevices()
{
    qWarning() << Q_FUNC_INFO;
    auto ret = &m_connectedDevices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

QAbstractItemModel * Displays::getDisconnectedDevices()
{
    qWarning() << Q_FUNC_INFO;
    auto ret = &m_disconnectedDevices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}
*/

/***
****
***/

/*
void Displays::disconnectDevice()
{
    qWarning() << Q_FUNC_INFO;
}

void Displays::connectDevice(const QString &address)
{
    qWarning() << Q_FUNC_INFO;
}

void Displays::removeDevice()
{
    qWarning() << Q_FUNC_INFO;
}
*/
