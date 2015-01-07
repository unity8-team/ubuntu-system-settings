/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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

#ifndef NETWORKABOUT_H
#define NETWORKABOUT_H

#include <QObject>
#include <QStringList>
#include <QtDBus/QDBusInterface>


class NetworkAbout : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString bluetoothMacAddress
                READ bluetoothMacAddress
                NOTIFY bluetoothMacAddressChanged )

public:
    explicit NetworkAbout (QObject *parent = 0);
    QString bluetoothMacAddress();

Q_SIGNALS:
    void bluetoothMacAddressChanged(QString addresses);

private:
    QString m_bluetoothMacAddress;
    void setupBluetoothMacAddress();
    QDBusConnection m_systemBusConnection;
};

#endif // NETWORKABOUT_H
