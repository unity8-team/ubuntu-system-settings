/*
 * Copyright (C) 2016 Canonical Ltd
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
}

Displays::Displays(const QDBusConnection &dbus, QObject *parent):
    QObject(parent),
    m_dbus(dbus),
    m_devices(m_dbus)
{
    m_manager = new AethercastManager(AETHERCAST_SERVICE, AETHERCAST_PATH, m_dbus);

    m_aethercastProperties.reset(new FreeDesktopProperties(AETHERCAST_SERVICE, AETHERCAST_PATH, m_dbus));

    QObject::connect(m_aethercastProperties.data(), SIGNAL(PropertiesChanged(const QString&, const QVariantMap&, const QStringList&)),
                     this, SLOT(slotPropertiesChanged(const QString&, const QVariantMap&, const QStringList&)));


    watchCall(m_aethercastProperties->GetAll(AETHERCAST_MANAGER_IFACE), [=](QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<QVariantMap> reply = *watcher;

        if (reply.isError()) {
            qWarning() << "Failed to retrieve properties for manager";
            watcher->deleteLater();
            return;
        }

        auto properties = reply.argumentAt<0>();
        setProperties(properties);
        watcher->deleteLater();
    });

    m_connectedDevices.filterOnStates(Device::State::Configuration |
                                      Device::State::Connected);
    m_connectedDevices.setSourceModel(&m_devices);

    m_disconnectedDevices.filterOnStates(Device::State::Idle |
                                         Device::State::Disconnected);
    m_disconnectedDevices.setSourceModel(&m_devices);
    connect(&m_connectedDevices, SIGNAL(rowsInserted(const QModelIndex, int, int)), this, SIGNAL(connectedDevicesChanged()));
    connect(&m_disconnectedDevices, SIGNAL(rowsInserted(const QModelIndex, int, int)), this, SIGNAL(disconnectedDevicesChanged()));
}

void Displays::slotPropertiesChanged(const QString &interface, const QVariantMap &changedProperties,
                                           const QStringList &invalidatedProperties)
{
    Q_UNUSED(invalidatedProperties);

    if (interface != AETHERCAST_MANAGER_IFACE)
        return;

    setProperties(changedProperties);
}

void Displays::setProperties(const QMap<QString,QVariant> &properties)
{
    QMapIterator<QString,QVariant> it(properties);
    while (it.hasNext()) {
        it.next();
        updateProperty(it.key(), it.value());
    }
}

void Displays::setEnabled(bool enabled)
{
    if (!m_manager)
        return;
    m_manager->setEnabled(enabled);
}

void Displays::scan()
{
    if (!m_manager)
        return;
    m_manager->Scan();
}

QAbstractItemModel * Displays::devices()
{
    auto ret = &m_devices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

QAbstractItemModel * Displays::connectedDevices()
{
    auto ret = &m_connectedDevices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

QAbstractItemModel * Displays::disconnectedDevices()
{
    auto ret = &m_disconnectedDevices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

void Displays::handleConnectError(QDBusError error)
{
    if (error.name() == "org.aethercast.Error.None")
        Q_EMIT(connectError(Error::None));
    else if (error.name() == "org.aethercast.Error.Failed")
        Q_EMIT(connectError(Error::Failed));
    else if (error.name() == "org.aethercast.Error.Already")
        Q_EMIT(connectError(Error::Already));
    else if (error.name() == "org.aethercast.Error.ParamInvalid")
        Q_EMIT(connectError(Error::ParamInvalid));
    else if (error.name() == "org.aethercast.Error.InvalidState")
        Q_EMIT(connectError(Error::InvalidState));
    else if (error.name() == "org.aethercast.Error.NotConnected")
        Q_EMIT(connectError(Error::NotConnected));
    else if (error.name() == "org.aethercast.Error.NotReady")
        Q_EMIT(connectError(Error::NotReady));
    else
        Q_EMIT(connectError(Error::Unknown));
}

void Displays::disconnectDevice(const QString &address)
{
    auto device = m_devices.getDeviceFromAddress(address);
    if (!device)
        return;
    QDBusPendingReply<void> reply = device->disconnect();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(callFinishedSlot(QDBusPendingCallWatcher*)));
}

void Displays::callFinishedSlot(QDBusPendingCallWatcher *call)
{
    qWarning() << Q_FUNC_INFO;
    QDBusPendingReply<QString, QByteArray> reply = *call;
    if (reply.isError())
        handleConnectError(reply.error());
    call->deleteLater();
}

void Displays::connectDevice(const QString &address)
{
    qWarning() << Q_FUNC_INFO;
    auto device = m_devices.getDeviceFromAddress(address);
    if (!device)
        return;
    QDBusPendingReply<void> reply = device->connect();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(callFinishedSlot(QDBusPendingCallWatcher*)));
}

void Displays::updateProperty(const QString &key, const QVariant &value)
{
    if (key == "Scanning") 
        Q_EMIT(scanningChanged(value.toBool()));
    if (key == "State") 
        Q_EMIT(stateChanged());
    if (key == "Enabled") 
        Q_EMIT(enabledChanged(value.toBool()));
}
