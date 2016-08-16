/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FAKE_NETWORK_MANAGER_H
#define FAKE_NETWORK_MANAGER_H

#include <QObject>
#include <QVariant>
#include <QString>
#include <QDBusConnection>
#include <QDBusInterface>

#include <libqtdbusmock/DBusMock.h>
#include <libqtdbustest/DBusTestRunner.h>

#define NM_SERVICE "org.freedesktop.NetworkManager"
#define NM_IFACE "org.freedesktop.NetworkManager"
#define NM_MAIN_OBJECT "/org/freedesktop/NetworkManager"
#define FREEDESKTOP_PROPERTIES_IFACE "org.freedesktop.DBus.Properties"
// #define NM_AP_IFACE "org.freedesktop.NetworkManager.AccessPoint"
// #define NM_DEVICE_IFACE "org.freedesktop.NetworkManager.Device"
// #define NM_DEVICE_WIRELESS_IFACE "org.freedesktop.NetworkManager.Device.Wireless"
// #define NM_ACTIVE_CONNECTION_IFACE "org.freedesktop.NetworkManager.Connection.Active"

using namespace QtDBusTest;
using namespace QtDBusMock;

class FakeNetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit FakeNetworkManager(const QVariantMap &parameters, QObject *parent = 0);
    ~FakeNetworkManager();

    const QDBusConnection & dbus() { return m_dbusTestRunner.systemConnection(); }
    QVariant getProperty(const QString &path,
                         const QString &interface,
                         const QString &property);
private:
    DBusTestRunner m_dbusTestRunner;
    DBusMock m_dbusMock;
    QDBusInterface *m_nmMock;
};

#endif // FAKE_NETWORK_MANAGER_H
