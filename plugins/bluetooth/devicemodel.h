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
 * Charles Kerr <charles.kerr@canonical.com>
 */

#ifndef BLUETOOTH_DEVICE_MODEL_H
#define BLUETOOTH_DEVICE_MODEL_H

#include <QByteArray>
#include <QHash>
#include <QList>
#include <QVariant>

#include <QAbstractListModel>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "device.h"

class DeviceModel: public QAbstractListModel
{
    Q_OBJECT

public:
    DeviceModel(QDBusConnection &dbus, QObject *parent = 0);
    ~DeviceModel();

    enum Roles
    {
      // Qt::DisplayRole holds device name
      // Qt::DecorationRole has icon
      TypeRole = Qt::UserRole,
      StrengthRole,
      ConnectionRole,
      AddressRole,
      LastRole = AddressRole
    };

    // implemented virtual methods from QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int,QByteArray> roleNames() const;

    QSharedPointer<Device> getDeviceFromAddress(const QString &address);
    QSharedPointer<Device> getDeviceFromPath(const QString &path);

public:
    void pairDevice(const QString &address);

private:
    QDBusConnection m_dbus;
    QDBusInterface m_bluezManager;

    QScopedPointer<QDBusInterface> m_bluezAdapter;
    void clearAdapter();
    void setAdapterFromPath(const QString &objectPath);

    QList<QSharedPointer<Device> > m_devices;
    void updateDevices();
    void addDevice(QSharedPointer<Device> &device);
    void addDevice(const QString &objectPath);
    void removeRow(int i);
    int findRowFromAddress(const QString &address) const;
    void emitRowChanged(int row);

private Q_SLOTS:
    void slotDeviceChanged();
    void slotDeviceCreated(const QDBusObjectPath &);
    void slotDeviceRemoved(const QDBusObjectPath &);
    void slotDeviceFound(const QString &, const QMap<QString,QVariant>&);
    void slotDeviceDisappeared(const QString&);
};

class DeviceFilter: public QSortFilterProxyModel
{
    Q_OBJECT

public:
    DeviceFilter() {}
    virtual ~DeviceFilter() {}
    void filterOnType(Device::Type);
    void filterOnConnections(Device::Connections);

protected:
    virtual bool filterAcceptsRow(int, const QModelIndex&) const;
    virtual bool lessThan(const QModelIndex&, const QModelIndex&) const;

private:
    Device::Type m_type = Device::Type::Other;
    bool m_typeEnabled = false;
    Device::Connections m_connections = Device::Connection::Connected;
    bool m_connectionsEnabled = false;
};

#endif // BLUETOOTH_DEVICE_MODEL_H
