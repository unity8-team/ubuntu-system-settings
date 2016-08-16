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

#ifndef MOCK_WIFI_DBUS_HELPER_H
#define MOCK_WIFI_DBUS_HELPER_H

#include <QObject>
#include <QStringList>
#include <QVariantMap>

class MockDbusHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString wifiIp4Address READ getWifiIpAddress
               NOTIFY wifiIp4AddressChanged)
public:
    explicit MockDbusHelper(QObject *parent = nullptr);
    ~MockDbusHelper() {};

    Q_INVOKABLE void connect(QString ssid, int security, int auth, QStringList usernames, QStringList password, QStringList certs, int p2auth);
    Q_INVOKABLE QList<QStringList> getPreviouslyConnectedWifiNetworks();
    Q_INVOKABLE void forgetConnection(const QString dbus_path);
    Q_INVOKABLE bool forgetActiveDevice();
    QString getWifiIpAddress();

    Q_INVOKABLE void mockDeviceStateChanged(uint newState, uint reason);
    Q_INVOKABLE QVariantMap getConnectArguments(); // mock only
    Q_INVOKABLE bool getForgetActiveDeviceCalled(); // mock only

public Q_SLOTS:
    void nmDeviceStateChanged(uint, uint, uint);

Q_SIGNALS:
    void wifiIp4AddressChanged(QString wifiIp4Address);
    void deviceStateChanged(uint newState, uint reason);
private:
    // This stores params passed to connect().
    bool forgetActiveDeviceCalled = false;
    QVariantMap m_connect = QVariantMap();
};

#endif // MOCK_WIFI_DBUS_HELPER_H
