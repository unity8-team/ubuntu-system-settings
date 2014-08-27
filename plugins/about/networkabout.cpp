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

#include "networkabout.h"
#include <QDebug>

NetworkAbout::NetworkAbout(QObject *parent)
    : QObject(parent)
{

    setupNetworkMacAddresses();
    setupBluetoothMacAddress();
}

void NetworkAbout::setupNetworkMacAddresses()
{
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    int count = ifaces.count();

    QString str = QString();
    for(int i=0;i<count;i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        str = iface.hardwareAddress();
        // add interfaces that are not loopback
        if (!iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            m_networkMacAddresses << str;
        }
    }
}

void NetworkAbout::setupBluetoothMacAddress()
{
    QBluetoothLocalDevice localDevice;
    if (localDevice.isValid()) {
        qWarning() << "bt device valid";
        qWarning() << localDevice.address().toString();
        m_bluetoothMacAddress = localDevice.address().toString();
    } else {
        qWarning() << "bt device invalid";

    }
    qWarning() << "number of bt devices:" << QBluetoothLocalDevice::allDevices().count();
    // if (!QBluetoothLocalDevice::allDevices().count()) {
    //     qWarning() << "no bt devices";
    //     qWarning() << localDevice.address().toString();
    //     return;
    // }

}

QStringList NetworkAbout::networkMacAddresses()
{
    return m_networkMacAddresses;
}

QString NetworkAbout::bluetoothMacAddress()
{
    return m_bluetoothMacAddress;
}
