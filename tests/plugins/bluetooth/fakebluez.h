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

#ifndef FAKEBLUEZ_H
#define FAKEBLUEZ_H

#include <QObject>
#include <QVariant>
#include <QString>
#include <QDBusConnection>
#include <QDBusInterface>

#include <libqtdbusmock/DBusMock.h>
#include <libqtdbustest/DBusTestRunner.h>

#define BLUEZ_SERVICE "org.bluez"
#define BLUEZ_MAIN_OBJECT "/"
#define BLUEZ_MOCK_IFACE "org.bluez.Mock"

#define BLUEZ_MANAGER_IFACE "org.bluez.Manager"
#define BLUEZ_ADAPTER_IFACE "org.bluez.Adapter"
#define BLUEZ_DEVICE_IFACE "org.bluez.Device"
#define BLUEZ_AUDIO_IFACE "org.bluez.Audio"

using namespace QtDBusTest;
using namespace QtDBusMock;

namespace Bluez {

class FakeBluez : public QObject
{
    Q_OBJECT

private:
    DBusMock m_dbusMock;
    DBusTestRunner m_dbusTestRunner;

    QDBusInterface *m_bluezMock;
    QString m_currentAdapter;
    QList<QString> m_devices;

    QDBusInterface getInterface(const QString &path, const QString &interface);

public:
    explicit FakeBluez(QObject *parent = 0);

    const QString currentAdapter() { return m_currentAdapter; }
    const QList<QString> devices() { return m_devices; }
    const QDBusConnection & dbus() { return m_dbusTestRunner.systemConnection(); }

    QString addAdapter(const QString &name, const QString &system_name);
    QString addDevice(const QString &name, const QString &address);

    QVariant getProperty(const QString &path,
                         const QString &interface,
                         const QString &property);
    
    void setProperty(const QString &path,
                     const QString &interface,
                     const QString &property,
                     const QVariant &value);

};

}

#endif // FAKEBLUEZ_H
