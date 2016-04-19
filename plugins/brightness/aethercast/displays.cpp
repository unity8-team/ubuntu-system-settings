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
    qWarning() << Q_FUNC_INFO;
}

Displays::Displays(const QDBusConnection &dbus, QObject *parent):
    QObject(parent),
    m_dbus(dbus),
    m_devices(m_dbus)
{
    qWarning() << Q_FUNC_INFO;

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

    m_connectedDevices.filterOnState("connected");
    m_connectedDevices.setSourceModel(&m_devices);
}

void Displays::slotPropertiesChanged(const QString &interface, const QVariantMap &changedProperties,
                                           const QStringList &invalidatedProperties)
{
    Q_UNUSED(invalidatedProperties);
    qWarning() << Q_FUNC_INFO;

    if (interface != AETHERCAST_MANAGER_IFACE)
        return;

    setProperties(changedProperties);
}

void Displays::setProperties(const QMap<QString,QVariant> &properties)
{
    qWarning() << Q_FUNC_INFO;
    QMapIterator<QString,QVariant> it(properties);
    while (it.hasNext()) {
        it.next();
        updateProperty(it.key(), it.value());
    }
}

void Displays::scan()
{
    qWarning() << Q_FUNC_INFO;
    if (!m_manager)
        return;
    m_manager->Scan();
}

QAbstractItemModel * Displays::devices()
{
    qWarning() << Q_FUNC_INFO;
    auto ret = &m_devices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

QAbstractItemModel * Displays::connectedDevices()
{
    qWarning() << Q_FUNC_INFO;
    auto ret = &m_connectedDevices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

void Displays::disconnectDevice(const QString &address)
{
    qWarning() << Q_FUNC_INFO << address;
    auto device = m_devices.getDeviceFromAddress(address);
    if (device)
        device->disconnect();
}

void Displays::connectDevice(const QString &address)
{
    qWarning() << Q_FUNC_INFO << address;
    auto device = m_devices.getDeviceFromAddress(address);
    if (device)
        device->connect();
}

void Displays::updateProperty(const QString &key, const QVariant &value)
{
    qWarning() << Q_FUNC_INFO << key << ":" << value;
    if (key == "Scanning") 
        Q_EMIT(scanningChanged(value.toBool()));
    if (key == "State") 
        Q_EMIT(stateChanged());
}
