/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QDBusReply>
#include <QDebug>

#include "fakebluez.h"

namespace Bluez {

FakeBluez::FakeBluez(QObject *parent) :
    QObject(parent),
    m_dbusMock(m_dbusTestRunner)
{
    DBusMock::registerMetaTypes();

    m_dbusMock.registerTemplate(BLUEZ_SERVICE, "bluez4",
                                QDBusConnection::SystemBus);
    m_dbusTestRunner.startServices();

    m_bluezMock = new QDBusInterface(BLUEZ_SERVICE,
                                     BLUEZ_MAIN_OBJECT,
                                     BLUEZ_MOCK_IFACE,
                                     m_dbusTestRunner.systemConnection());
}

QString
FakeBluez::addAdapter(const QString &name, const QString &system_name)
{
    QDBusReply<QString> reply = m_bluezMock->call("AddAdapter",
                                                    name, system_name);

    if (reply.isValid()) {
        m_currentAdapter = reply.value().replace("/org/bluez/", "");
    } else {
        qWarning() << "Failed to add mock adapter:" << reply.error().message();
    }

    return reply.isValid() ? reply.value() : QString();
}

QString
FakeBluez::addDevice(const QString& name, const QString &address)
{
    QDBusReply<QString> reply = m_bluezMock->call("AddDevice",
                                                  m_currentAdapter,
                                                  address, name);

    if (reply.isValid()) {
        m_devices.append(reply.value());
    } else {
        qWarning() << "Failed to add mock device:" << reply.error().message();
    }

    return reply.isValid() ? reply.value() : QString();
}

QVariant
FakeBluez::getProperty(const QString &path,
                       const QString &interface,
                       const QString &property)
{
    QDBusInterface iface(BLUEZ_SERVICE, path,
                         "org.freedesktop.DBus.Properties",
                         m_dbusTestRunner.systemConnection());

    QDBusReply<QVariant> reply = iface.call("Get", interface, property);

    if (reply.isValid()) {
        return reply.value();
    } else {
        qWarning() << "Error getting property from mock:"
                   << reply.error().message();
    }

    return reply.isValid() ? reply.value() : QVariant();
}

void
FakeBluez::setProperty(const QString &path,
                       const QString &interface,
                       const QString &property,
                       const QVariant &value)
{
    QDBusInterface iface(BLUEZ_SERVICE, path,
                         interface,
                         m_dbusTestRunner.systemConnection());

    QDBusReply<void> reply = iface.call("SetProperty",
                                        property, value);

    if (!reply.isValid()) {
        qWarning() << "Error setting property on mock:"
                   << reply.error().message();
    }
}

}
