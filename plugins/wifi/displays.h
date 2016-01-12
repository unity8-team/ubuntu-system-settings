/*
 * Copyright (C) 2015 Canonical Ltd
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

#ifndef DISPLAYS_H
#define DISPLAYS_H

#include <QObject>
#include <QtDBus>
#include "devicemodel.h"
#include "aethercast_device.h"
#include "aethercast_manager.h"

class Displays : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QAbstractItemModel* devices
                READ devices
                NOTIFY devicesChanged)

    Q_PROPERTY (QAbstractItemModel* connectedDevices
                READ getConnectedDevices
                CONSTANT)

    Q_PROPERTY (QAbstractItemModel* disconnectedDevices
                READ getDisconnectedDevices
                CONSTANT)

    Q_PROPERTY (bool discovering
                READ isDiscovering
                NOTIFY discoveringChanged)

Q_SIGNALS:
    void devicesChanged();
    void discoveringChanged(bool isActive);

public:
    explicit Displays(QObject *parent = nullptr);
    explicit Displays(const QDBusConnection &dbus, QObject *parent = nullptr);
    ~Displays() {}

    Q_INVOKABLE void connectDevice(const QString &address);
    Q_INVOKABLE void disconnectDevice();
    Q_INVOKABLE void toggleDiscovery();
    Q_INVOKABLE void startDiscovery();
    Q_INVOKABLE void stopDiscovery();
    Q_INVOKABLE void scan();

public:
    QAbstractItemModel * devices();
    QAbstractItemModel * getConnectedDevices();
    QAbstractItemModel * getDisconnectedDevices();
    QAbstractItemModel * getAutoconnectDevices();

    bool isDiscovering() const { return true;/*m_devices.isDiscovering();*/ }

private:
    QDBusConnection m_dbus;
    DeviceModel m_devices;
    DeviceFilter m_connectedDevices;
    DeviceFilter m_disconnectedDevices;
    AethercastManager* m_manager;
};

#endif // DISPLAYS_H

