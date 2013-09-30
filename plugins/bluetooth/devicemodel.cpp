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
 *
*/

#include <QDBusReply>

#include "devicemodel.h"
#include "dbus-shared.h"

namespace
{
  const int SCANNING_ACTIVE_DURATION_MSEC = (10 * 1000);

  const int SCANNING_IDLE_DURATION_MSEC = (10 * 1000);
}

/***
****
***/

DeviceModel::DeviceModel(QDBusConnection &dbus, QObject *parent):
    QAbstractListModel(parent),
    m_dbus(dbus),
    m_bluezManager("org.bluez", "/", "org.bluez.Manager", m_dbus)
{
    if (m_bluezManager.isValid()) {
        QDBusReply<QDBusObjectPath> qObjectPath = m_bluezManager.call("DefaultAdapter");
        if (qObjectPath.isValid())
            setAdapterFromPath(qObjectPath.value().path());
    }

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
}

DeviceModel::~DeviceModel()
{
    clearAdapter();
}

int DeviceModel::findRowFromAddress(const QString &address) const
{
    for (int i=0, n=m_devices.size(); i<n; i++)
        if (m_devices[i]->getAddress() == address)
            return i;

    return -1;
}

/***
****
***/

void DeviceModel::restartTimer()
{
    m_timer.start (m_isDiscovering ? SCANNING_ACTIVE_DURATION_MSEC
                                   : SCANNING_IDLE_DURATION_MSEC);
}

void DeviceModel::stopDiscovery()
{
    if (m_isDiscovering) {
        if (m_bluezAdapter)
            m_bluezAdapter->asyncCall("StopDiscovery");
        m_isDiscovering = false;
        Q_EMIT(discoveringChanged(m_isDiscovering));
    }

    restartTimer();
}

void DeviceModel::startDiscovery()
{
    if (m_bluezAdapter && !m_isDiscovering) {
        m_bluezAdapter->asyncCall("StartDiscovery");
        m_isDiscovering = true;
        Q_EMIT(discoveringChanged(m_isDiscovering));
    }

    restartTimer();
}

void DeviceModel::toggleDiscovery()
{
    if (isDiscovering())
        stopDiscovery();
    else
        startDiscovery();
}

void DeviceModel::slotTimeout()
{
    toggleDiscovery ();
}

/***
****
***/

void DeviceModel::clearAdapter()
{
    if (m_bluezAdapter) {

        QDBusConnection bus = m_bluezAdapter->connection();
        const QString service = m_bluezAdapter->service();
        const QString path = m_bluezAdapter->path();
        const QString interface = m_bluezAdapter->interface();

        stopDiscovery();

        bus.disconnect(service, path, interface, "DeviceCreated",
                       this, SLOT(slotDeviceCreated(const QDBusObjectPath&)));
        bus.disconnect(service, path, interface, "DeviceRemoved",
                       this, SLOT(slotDeviceRemoved(const QDBusObjectPath&)));
        bus.disconnect(service, path, interface, "DeviceFound",
                       this, SLOT(slotDeviceFound(const QString&, const QMap<QString,QVariant>&)));
        bus.disconnect(service, path, interface, "DeviceDisappeared",
                       this, SLOT(slotDeviceDisappeared(const QString&)));

        m_bluezAdapter.reset(0);
    }
}

void DeviceModel::setAdapterFromPath(const QString &path)
{
    clearAdapter();

    if (!path.isEmpty()) {

        const QString service = "org.bluez";
        const QString interface = "org.bluez.Adapter";
        auto i = new QDBusInterface(service, path, interface, m_dbus);

        m_dbus.connect(service, path, interface, "DeviceCreated", 
                       this, SLOT(slotDeviceCreated(const QDBusObjectPath&)));
        m_dbus.connect(service, path, interface, "DeviceRemoved", 
                       this, SLOT(slotDeviceRemoved(const QDBusObjectPath&)));
        m_dbus.connect(service, path, interface, "DeviceFound", 
                       this, SLOT(slotDeviceFound(const QString&, const QMap<QString,QVariant>&)));
        m_dbus.connect(service, path, interface, "DeviceDisappeared", 
                       this, SLOT(slotDeviceDisappeared(const QString&)));

        m_bluezAdapter.reset(i);
        startDiscovery();
        updateDevices();
    }
}

void DeviceModel::updateDevices()
{
    if (m_bluezAdapter && m_bluezAdapter->isValid()) {
        QDBusReply<QList<QDBusObjectPath> > reply = m_bluezAdapter->call("ListDevices");
        if (reply.isValid())
            for (auto path : reply.value())
                addDevice(path.path());
    }
}

/***
****
***/

void DeviceModel::addDevice(const QString &path)
{
    QSharedPointer<Device> device(new Device(path, m_dbus));
    if (device->isValid()) {
        QObject::connect(device.data(), SIGNAL(deviceChanged()),
                         this, SLOT(slotDeviceChanged()));
        addDevice(device);
    }
}

void DeviceModel::addDevice(QSharedPointer<Device> &device)
{
    int row = findRowFromAddress(device->getAddress());

    if (row >= 0) { // update existing device
        m_devices[row] = device;
        emitRowChanged(row);
    } else { // add new device
        row = m_devices.size();
        beginInsertRows(QModelIndex(), row, row);
        m_devices.append(device);
        endInsertRows();
    }
}

void DeviceModel::removeRow(int row)
{
    if (0<=row && row<m_devices.size()) {
        beginRemoveRows(QModelIndex(), row, row);
        m_devices.removeAt(row);
        endRemoveRows();
    }
}

void DeviceModel::emitRowChanged(int row)
{
    if (0<=row && row<m_devices.size()) {
        QModelIndex qmi = index(row, 0);
        Q_EMIT(dataChanged(qmi, qmi));
    }
}

/***
****
***/

void DeviceModel::slotDeviceCreated(const QDBusObjectPath &path)
{
    addDevice(path.path());
}

void DeviceModel::slotDeviceFound(const QString                &address,
                                  const QMap<QString,QVariant> &properties)
{
    Q_UNUSED(properties);

    auto device = getDeviceFromAddress(address);
    if (!device) // hey, we haven't seen this one before
        m_bluezAdapter->asyncCall(QLatin1String("CreateDevice"), address);
}

void DeviceModel::slotDeviceRemoved(const QDBusObjectPath &path)
{
    Q_UNUSED(path);

    /* This is a no-op because we want to list both paired & unpaired devices.
       So, keep it in m_devices until a call to slotDeviceDisappeared()
       indicates the device has disappeared altogether */
}

void DeviceModel::slotDeviceDisappeared(const QString &address)
{
    const int row = findRowFromAddress(address);
    if ((row >= 0) && !m_devices[row]->isPaired())
        removeRow(row);
}

void DeviceModel::slotDeviceChanged()
{
    const Device * device = qobject_cast<Device*>(sender());

    // find the row that goes with this device
    int row = -1;
    if (device != nullptr)
        for (int i=0, n=m_devices.size(); row==-1 && i<n; i++)
            if (m_devices[i].data() == device)
                row = i;

    if (row != -1)
        emitRowChanged(row);
}

QSharedPointer<Device> DeviceModel::getDeviceFromAddress(const QString &address)
{
    QSharedPointer<Device> device;

    const int row = findRowFromAddress(address);
    if (row >= 0)
        device = m_devices[row];

    return device;
}

QSharedPointer<Device> DeviceModel::getDeviceFromPath(const QString &path)
{
    for (auto device : m_devices)
        if (device->getPath() == path)
            return device;

    return QSharedPointer<Device>();
}

void DeviceModel::pairDevice (const QString &address)
{
    if (m_bluezAdapter) {
        m_bluezAdapter->asyncCall("CreatePairedDevice",
                                  address,
                                  qVariantFromValue(QDBusObjectPath(DBUS_AGENT_PATH)),
                                  QString(DBUS_AGENT_CAPABILITY));
    }
}

/***
****
***/

int
DeviceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_devices.size();
}

QHash<int,QByteArray> DeviceModel::roleNames() const
{
    static QHash<int,QByteArray> names;

    if (Q_UNLIKELY(names.empty())) {
        names[Qt::DisplayRole] = "displayName";
        names[Qt::DecorationRole] = "iconName";
        names[TypeRole] = "type";
        names[StrengthRole] = "strength";
        names[ConnectionRole] = "connection";
        names[AddressRole] = "addressName";
    }

    return names;
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;

    if ((0<=index.row()) && (index.row()<m_devices.size())) {

        auto device = m_devices[index.row()];

        switch (role) {
        case Qt::DisplayRole:
            ret = device->isPaired() ? device->getName() : device->getName() + "…";
            break;

        case Qt::DecorationRole:
            ret = device->getIconName();
            break;

        case TypeRole:
            ret = device->getType();
            break;

        case StrengthRole:
            ret = (int) device->getStrength();
            break;

        case ConnectionRole:
            ret = (int) device->getConnection();
            break;

        case AddressRole:
            ret = device->getAddress();
            break;
        }
    }

    return ret;
}


/***
****
****  Filter
****
***/

void DeviceFilter::filterOnType(Device::Type type)
{
    m_type = type;
    m_typeEnabled = true;
    invalidateFilter();
}

void DeviceFilter::filterOnConnections(Device::Connections connections)
{
    m_connections = connections;
    m_connectionsEnabled = true;
    invalidateFilter();
}

bool DeviceFilter::filterAcceptsRow(int sourceRow,
                                    const QModelIndex &sourceParent) const
{
    bool accepts = true;
    QModelIndex childIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    if (accepts && m_typeEnabled) {
        const int type = childIndex.model()->data(childIndex, DeviceModel::TypeRole).value<int>();
        accepts = type == m_type;
    }

    if (accepts && m_connectionsEnabled) {
        const int connection = childIndex.model()->data(childIndex, DeviceModel::ConnectionRole).value<int>();
        accepts = (m_connections & connection) != 0;
    }

    return accepts;
}

bool DeviceFilter::lessThan(const QModelIndex &left,
                            const QModelIndex &right) const
{
  const QString a = sourceModel()->data(left, Qt::DisplayRole).value<QString>();
  const QString b = sourceModel()->data(right, Qt::DisplayRole).value<QString>();
  return a < b;
}
