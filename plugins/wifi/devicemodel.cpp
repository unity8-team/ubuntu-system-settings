/*
 * Copyright (C) 2016 Canonical Ltd
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

#include "devicemodel.h"

#include <QDBusReply>
#include <QDebug>

#include "dbus-shared.h"

namespace
{
  const int SCANNING_ACTIVE_DURATION_MSEC = (30 * 1000);
  const int SCANNING_IDLE_DURATION_MSEC = (10 * 1000);
}

DeviceModel::DeviceModel(QDBusConnection &dbus, QObject *parent):
    QAbstractListModel(parent),
    m_dbus(dbus),
    m_aethercastManager(AETHERCAST_SERVICE, "/org/aethercast", m_dbus),
    m_isDiscovering(false)
{
    qWarning() << Q_FUNC_INFO;
    if (m_aethercastManager.isValid()) {

        connect(&m_aethercastManager, SIGNAL(InterfacesAdded(const QDBusObjectPath&, InterfaceList)),
                this, SLOT(slotInterfacesAdded(const QDBusObjectPath&, InterfaceList)));

        connect(&m_aethercastManager, SIGNAL(InterfacesRemoved(const QDBusObjectPath&, const QStringList&)),
                this, SLOT(slotInterfacesRemoved(const QDBusObjectPath&, const QStringList&)));

        watchCall(m_aethercastManager.GetManagedObjects(), [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<ManagedObjectList> reply = *watcher;

            if (reply.isError()) {
                qWarning() << "Failed to retrieve list of managed objects from Aethercast service: "
                           << reply.error().message();
                watcher->deleteLater();
                return;
            }

            auto objectList = reply.argumentAt<0>();

            for (QDBusObjectPath path : objectList.keys()) {
                InterfaceList ifaces = objectList.value(path);

                //qWarning() << Q_FUNC_INFO << path;
                break;
            }

            watcher->deleteLater();
        });
    }

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
}

DeviceModel::~DeviceModel()
{
    qWarning() << "Releasing device model ..";
}

void DeviceModel::slotInterfacesAdded(const QDBusObjectPath &objectPath, InterfaceList ifacesAndProps)
{
    Q_UNUSED(ifacesAndProps);

    qWarning() << Q_FUNC_INFO << objectPath.path();

    auto candidatedPath = objectPath.path();

    // At this point we can only get new devices
    if (!candidatedPath.startsWith(m_aethercastManager.path()))
        return;

    if (!ifacesAndProps.contains(AETHERCAST_DEVICE_IFACE))
        return;

    addDevice(candidatedPath, ifacesAndProps.value(AETHERCAST_DEVICE_IFACE));
}

void DeviceModel::slotInterfacesRemoved(const QDBusObjectPath &objectPath, const QStringList &interfaces)
{
    auto candidatedPath = objectPath.path();

    if (!m_aethercastManager.isValid())
        return;

    if (candidatedPath == m_aethercastManager.path() &&
        interfaces.contains(AETHERCAST_MANAGER_IFACE)) {
        return;
    }

    if (!candidatedPath.startsWith(m_aethercastManager.path()))
        return;

    if (!interfaces.contains(AETHERCAST_DEVICE_IFACE))
        return;

    auto device = getDeviceFromPath(candidatedPath);
    if (!device)
        return;

    const int row = findRowFromAddress(device->getAddress());
    if ((row >= 0))
        removeRow(row);
}

int DeviceModel::findRowFromAddress(const QString &address) const
{
    for (int i=0, n=m_devices.size(); i<n; i++)
        if (m_devices[i]->getAddress() == address)
            return i;

    return -1;
}

void DeviceModel::setDiscovering(bool value)
{
    if (value == m_isDiscovering)
        return;

    m_isDiscovering = value;
    Q_EMIT(discoveringChanged(m_isDiscovering));
}

void DeviceModel::stopDiscovery()
{
    /*
    if (m_aethercastManager.isValid() && m_isDiscovering) {

         watchCall(m_aethercastManager.StopDiscovery(), [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<void> reply = *watcher;
            if (reply.isError()) {
                qWarning() << "Failed to stop device discovery:"
                           << reply.error().message();
            }

            watcher->deleteLater();
        });
    }
    */
}

void DeviceModel::startDiscovery()
{
    /*
    if (m_aethercastManager.isValid() && !m_isDiscovering) {

        watchCall(m_aethercastManager->StartDiscovery(), [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<void> reply = *watcher;
            if (reply.isError()) {
                qWarning() << "Failed to start device discovery:"
                           << reply.error().message();
            }

            watcher->deleteLater();
        });
    }
    */
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

void DeviceModel::setProperties(const QMap<QString,QVariant> &properties)
{
    QMapIterator<QString,QVariant> it(properties);
    while (it.hasNext()) {
        it.next();
        updateProperty(it.key(), it.value());
    }
}

void DeviceModel::updateProperty(const QString &key, const QVariant &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
    /*
    if (key == "Name") {
        m_adapterName = value.toString();
    } else if (key == "Address") {
        m_adapterAddress = value.toString();
    }
    */
}

void DeviceModel::slotPropertyChanged(const QString      &key,
                                      const QDBusVariant &value)
{
    updateProperty (key, value.variant());
}

void DeviceModel::slotDevicePairingDone(bool success)
{
    Device *device = static_cast<Device*>(sender());

    Q_EMIT(devicePairingDone(device, success));
}

void DeviceModel::addDevice(const QString &path, const QVariantMap &properties)
{
    qWarning() << Q_FUNC_INFO;
    QSharedPointer<Device> device(new Device(path, m_dbus));
    device->setProperties(properties);
    qWarning() << Q_FUNC_INFO;

    if (device) {
        QObject::connect(device.data(), SIGNAL(deviceChanged()),
                         this, SLOT(slotDeviceChanged()));
        //QObject::connect(device.data(), SIGNAL(pairingDone(bool)),
        //                 this, SLOT(slotDevicePairingDone(bool)));
        addDevice(device);
    }
}

void DeviceModel::addDevice(QSharedPointer<Device> &device)
{
    qWarning() << Q_FUNC_INFO;
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

void DeviceModel::slotRemoveFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<void> reply = *call;

    if (reply.isError()) {
        qWarning() << "Could not remove device:" << reply.error().message();
    }
    call->deleteLater();
}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_devices.size();
}

QHash<int,QByteArray> DeviceModel::roleNames() const
{
    static QHash<int,QByteArray> names;

    if (Q_UNLIKELY(names.empty())) {
        names[Qt::DisplayRole] = "displayName";
        names[StateRole] = "state";
        names[AddressRole] = "addressName";
    }

    return names;
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;

    if ((0<=index.row()) && (index.row()<m_devices.size())) {

        auto device = m_devices[index.row()];
        QString displayName;

        switch (role) {
        case Qt::DisplayRole:
            displayName = device->getName();

            if (displayName.isEmpty())
                displayName = device->getAddress();

            ret = displayName;
            break;

        case StateRole:
            ret = device->getState();
            break;

        case AddressRole:
            ret = device->getAddress();
            break;

        }
    }

    return ret;
}

bool DeviceFilter::lessThan(const QModelIndex &left,
                            const QModelIndex &right) const
{
  const QString a = sourceModel()->data(left, Qt::DisplayRole).value<QString>();
  const QString b = sourceModel()->data(right, Qt::DisplayRole).value<QString>();
  return a < b;
}
