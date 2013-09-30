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
 * Sebastien Bacher <sebastien.bacher@canonical.com>
 *
*/

#ifndef BATTERY_H
#define BATTERY_H

#include <QDBusInterface>
#include <QObject>

#include <libupower-glib/upower.h>
#include <nm-client.h>

class Battery : public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool powerdRunning
                READ powerdRunning
                CONSTANT)

    Q_PROPERTY( QString deviceString
                READ deviceString
                CONSTANT)

    Q_PROPERTY( int lastFullCharge
                READ lastFullCharge
                NOTIFY lastFullChargeChanged)

    Q_PROPERTY( bool wifiEnabled
                READ getWifiEnabled
                WRITE setWifiEnabled
                NOTIFY wifiEnabledChanged)

public:
    explicit Battery(QObject *parent = 0);
    ~Battery();
    bool powerdRunning() const;
    QString deviceString() const;
    int lastFullCharge() const;
    Q_INVOKABLE QVariantList getHistory(const QString &deviceString, const int timespan, const int resolution);
    /* TODO: should be a dynamic property, or replaced by proper qt bindings */
    bool getWifiEnabled();
    void setWifiEnabled(bool enabled);

Q_SIGNALS:
    void lastFullChargeChanged();
    void wifiEnabledChanged();

private:
    QDBusConnection m_systemBusConnection;
    QString m_objectPath;
    QDBusInterface m_powerdIface;
    bool m_powerdRunning;
    UpDevice *m_device;
    QString m_deviceString;
    int m_lastFullCharge;
    NMClient *m_nm_client;
    void buildDeviceString();
    void getLastFullCharge();
    bool updateLastFullCharge(UpHistoryItem *item, int offset);
};

#endif // BATTERY_H
