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

#include "MockDbusHelper.h"

MockDbusHelper::MockDbusHelper(QObject *parent)
{
    Q_UNUSED(parent);
}

void MockDbusHelper::connect(QString ssid, int security, int auth, QStringList usernames, QStringList password, QStringList certs, int p2auth)
{
    m_connect["ssid"] = ssid;
    m_connect["security"] = security;
    m_connect["auth"] = auth;
    m_connect["usernames"] = usernames;
    m_connect["password"] = password;
    m_connect["certs"] = certs;
    m_connect["p2auth"] = p2auth;
}

QList<QStringList> MockDbusHelper::getPreviouslyConnectedWifiNetworks()
{
    return QList<QStringList>();
}

void MockDbusHelper::forgetConnection(const QString dbus_path)
{
    Q_UNUSED(dbus_path);
}

bool MockDbusHelper::forgetActiveDevice()
{
    forgetActiveDeviceCalled = true;
    return true;
}

void MockDbusHelper::nmDeviceStateChanged(uint, uint, uint)
{
}

QString MockDbusHelper::getWifiIpAddress()
{
    return QString();
}

void MockDbusHelper::mockDeviceStateChanged(uint newState, uint reason)
{
    Q_EMIT deviceStateChanged(newState, reason);
}

QVariantMap MockDbusHelper::getConnectArguments()
{
    return m_connect;
}

bool MockDbusHelper::getForgetActiveDeviceCalled()
{
    return forgetActiveDeviceCalled;
}

