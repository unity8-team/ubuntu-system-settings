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
 */

#include "aethercast_device.h"
#include "device.h"

#include <QDBusReply>
#include <QDebug> // qWarning()
#include <QThread>
#include <QTimer>

Device::Device(const QString &path, QDBusConnection &bus) :
   m_state(Device::Idle)
{
    initDevice(path, bus);
}

void Device::initDevice(const QString &path, QDBusConnection &bus)
{
    /* whenever any of the properties changes,
       trigger the catch-all deviceChanged() signal */
    QObject::connect(this, SIGNAL(nameChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(addressChanged()), this, SIGNAL(deviceChanged()));
    QObject::connect(this, SIGNAL(stateChanged()), this, SIGNAL(deviceChanged()));

    m_aethercastDevice.reset(new AethercastDevice(AETHERCAST_SERVICE, path, bus));
    /* Give our calls a bit more time than the default 25 seconds to
     * complete whatever they are doing. In some situations (e.g. with
     * specific devices) the default doesn't seem to be enough to. */
    m_aethercastDevice->setTimeout(130 * 1000 /* 130 seconds */);

    m_aethercastDeviceProperties.reset(new FreeDesktopProperties(AETHERCAST_SERVICE, path, bus));

    QObject::connect(m_aethercastDeviceProperties.data(), SIGNAL(PropertiesChanged(const QString&, const QVariantMap&, const QStringList&)),
                     this, SLOT(slotPropertiesChanged(const QString&, const QVariantMap&, const QStringList&)));

    Q_EMIT(pathChanged());

    watchCall(m_aethercastDeviceProperties->GetAll(AETHERCAST_DEVICE_IFACE), [=](QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<QVariantMap> reply = *watcher;

        if (reply.isError()) {
            qWarning() << "Failed to retrieve properties for device" << m_aethercastDevice->path();
            watcher->deleteLater();
            return;
        }

        auto properties = reply.argumentAt<0>();
        setProperties(properties);

        watcher->deleteLater();
    });
}

void Device::slotPropertiesChanged(const QString &interface, const QVariantMap &changedProperties,
                                   const QStringList &invalidatedProperties)
{
    Q_UNUSED(invalidatedProperties);

    if (interface != AETHERCAST_DEVICE_IFACE)
        return;

    setProperties(changedProperties);
}

void Device::setProperties(const QMap<QString,QVariant> &properties)
{
    QMapIterator<QString,QVariant> it(properties);
    while (it.hasNext()) {
        it.next();
        updateProperty(it.key(), it.value());
    }
}

QDBusPendingReply<void> Device::disconnect()
{
    return m_aethercastDevice->Disconnect();
}

QDBusPendingReply<void> Device::connect()
{
    return m_aethercastDevice->Connect(QString());
}

void Device::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        Q_EMIT(nameChanged());
    }
}

void Device::setAddress(const QString &address)
{
    if (m_address != address) {
        m_address = address;
        Q_EMIT(addressChanged());
    }
}

void Device::setState(const State &state)
{
    if (m_state != state) {
        m_state = state;
        Q_EMIT(stateChanged());
    }
}

void Device::updateProperty(const QString &key, const QVariant &value)
{
    if (key == "Name") {
        setName(value.toString());
    } else if (key == "Address") {
        setAddress(value.toString());
    } else if (key == "State") {
        if (value.toString() == "idle")
            setState(Device::Idle);
        else if (value.toString() == "disconnected")
            setState(Device::Disconnected);
        else if (value.toString() == "connected")
            setState(Device::Connected);
        else if (value.toString() == "association")
            setState(Device::Association);
        else if (value.toString() == "configuration")
            setState(Device::Configuration);
    }
}
