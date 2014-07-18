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
#include <QDebug>

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

        m_dbus.connect (m_bluezManager.service(),
                        m_bluezManager.path(),
                        m_bluezManager.interface(),
                        "DefaultAdapterChanged",
                        this, SLOT(slotDefaultAdapterChanged(const QDBusObjectPath&)));

        m_dbus.connect (m_bluezManager.service(),
                        m_bluezManager.path(),
                        m_bluezManager.interface(),
                        "AdapterRemoved",
                        this, SLOT(slotAdapterRemoved(const QDBusObjectPath&)));
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
    if (m_bluezAdapter && m_isPowered && !m_isDiscovering) {
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
        m_discoverableTimer.stop();
        trySetDiscoverable(false);

        bus.disconnect(service, path, interface, "DeviceCreated",
                       this, SLOT(slotDeviceCreated(const QDBusObjectPath&)));
        bus.disconnect(service, path, interface, "DeviceRemoved",
                       this, SLOT(slotDeviceRemoved(const QDBusObjectPath&)));
        bus.disconnect(service, path, interface, "DeviceFound",
                       this, SLOT(slotDeviceFound(const QString&, const QMap<QString,QVariant>&)));
        bus.disconnect(service, path, interface, "DeviceDisappeared",
                       this, SLOT(slotDeviceDisappeared(const QString&)));
        bus.disconnect(service, path, interface, "PropertyChanged",
                       this, SLOT(slotPropertyChanged(const QString&, const QDBusVariant&)));

        m_bluezAdapter.reset(0);
        m_adapterName.clear();

        beginResetModel();
        m_devices.clear();
        endResetModel();
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
        m_dbus.connect(service, path, interface, "PropertyChanged",
                       this, SLOT(slotPropertyChanged(const QString&, const QDBusVariant&)));

        m_bluezAdapter.reset(i);
        startDiscovery();
        updateDevices();

        QDBusReply<QMap<QString,QVariant> > properties = m_bluezAdapter->call("GetProperties");
        if (properties.isValid())
            setProperties(properties.value());

        // Delay enabling discoverability by 1 second.
        m_discoverableTimer.setSingleShot(true);
        connect(&m_discoverableTimer, SIGNAL(timeout()), this, SLOT(slotEnableDiscoverable()));
        m_discoverableTimer.start(1000);

    }
}

void DeviceModel::slotAdapterRemoved(const QDBusObjectPath &path)
{
  if (m_bluezAdapter && (m_bluezAdapter->path()==path.path()))
      clearAdapter();
}

void DeviceModel::slotDefaultAdapterChanged(const QDBusObjectPath &objectPath)
{
  setAdapterFromPath (objectPath.path());
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
    if (key == "Name") {
        m_adapterName = value.toString();
    } else if (key == "Address") {
        m_adapterAddress = value.toString();
    } else if (key == "Pairable") {
        m_isPairable = value.toBool();
    } else if (key == "Discoverable") {
        setDiscoverable(value.toBool());
    } else if (key == "Powered") {
        setPowered(value.toBool());
    }
}

void DeviceModel::setDiscoverable(bool discoverable)
{
    if (m_isDiscoverable != discoverable) {
        m_isDiscoverable = discoverable;
        Q_EMIT(discoverableChanged(m_isDiscoverable));
    }
}

void DeviceModel::setPowered(bool powered)
{
    if (m_isPowered != powered) {
        m_isPowered = powered;
        Q_EMIT(poweredChanged(m_isPowered));
    }
}

void DeviceModel::slotEnableDiscoverable()
{
    trySetDiscoverable(true);
}

void DeviceModel::trySetDiscoverable(bool discoverable)
{
    QVariant value;
    QDBusVariant disc(discoverable);
    QDBusReply<void > reply;

    value.setValue(disc);

    if (m_bluezAdapter && m_bluezAdapter->isValid()) {
        reply = m_bluezAdapter->call("SetProperty", "Discoverable", value);
        if (!reply.isValid())
            qWarning() << "Error setting device discoverable:" << reply.error();
    }
}

void DeviceModel::slotPropertyChanged(const QString      &key,
                                      const QDBusVariant &value)
{
    updateProperty (key, value.variant());
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
    const QString service = "org.bluez";
    const QString interface = "org.bluez.Device";
    auto bluezDevice = new QDBusInterface(service, path.path(), interface, m_dbus);

    // A device was created. Now, we likely already have it, so let's find it
    // again and finish the initialization.
    QDBusReply<QMap<QString,QVariant> > properties = bluezDevice->call("GetProperties");
    if (properties.isValid()) {
        QMapIterator<QString,QVariant> it(properties);
        while (it.hasNext()) {
            it.next();
            if (it.key() == "Address") {
                auto device = getDeviceFromAddress(it.value().toString());

                if (device) {
                    device->initDevice(path.path(), m_dbus);
                    QObject::connect(device.data(), SIGNAL(deviceChanged()),
                                     this, SLOT(slotDeviceChanged()));
                    addDevice(device);
                } else {
                    addDevice(path.path());
                }
                break;
            }
        }
    } else {
        qWarning() << "Invalid device properties for" << path.path();
    }
}

void DeviceModel::slotDeviceFound(const QString                &address,
                                  const QMap<QString,QVariant> &properties)
{
    Q_UNUSED(properties);

    auto device = getDeviceFromAddress(address);
    if (!device) {
        QSharedPointer<Device> device(new Device(properties));
        if (device->isValid()) {
            addDevice(device);
        }
    }
}

void DeviceModel::slotDeviceRemoved(const QDBusObjectPath &path)
{
    /* Remove the device immediately, it will be listed again
       once discovery results are returned.  */

    auto device = getDeviceFromPath(path.path());

    const int row = findRowFromAddress(device->getAddress());
    if ((row >= 0))
        removeRow(row);
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

void DeviceModel::addConnectAfterPairing(const QString &address, Device::ConnectionMode mode)
{
    auto device = getDeviceFromAddress(address);
    if (device) {
        device->addConnectAfterPairing(mode);
    } else {
        qWarning() << "Device could not be found, can't add an operation";
    }
}

void DeviceModel::slotCreateFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QDBusObjectPath> reply = *call;

    if (reply.isError()) {
        qWarning() << "Could not create device:" << reply.error().message();
    }

    call->deleteLater();
}

void DeviceModel::createDevice (const QString &address)
{
    if (m_bluezAdapter) {
        QDBusPendingCall pcall = m_bluezAdapter->asyncCall("CreatePairedDevice",
                                                           address,
                                                           qVariantFromValue(QDBusObjectPath(DBUS_AGENT_PATH)),
                                                           QString(DBUS_AGENT_CAPABILITY));

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
        QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                         this, SLOT(slotCreateFinished(QDBusPendingCallWatcher*)));
    } else {
        qWarning() << "Default adapter is not available for device creation";
    }
}

void DeviceModel::slotRemoveFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<void> reply = *call;

    if (reply.isError()) {
        qWarning() << "Could not remove device:" << reply.error().message();
    }
    call->deleteLater();
}

void DeviceModel::removeDevice (const QString &path)
{
    if (m_bluezAdapter) {
        QDBusPendingCall pcall = m_bluezAdapter->asyncCall("RemoveDevice", qVariantFromValue(QDBusObjectPath(path)));

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
        QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                         this, SLOT(slotRemoveFinished(QDBusPendingCallWatcher*)));
    } else {
        qWarning() << "Default adapter is not available for device removal";
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
        names[IconRole] = "iconPath";
        names[TypeRole] = "type";
        names[StrengthRole] = "strength";
        names[ConnectionRole] = "connection";
        names[AddressRole] = "addressName";
        names[TrustedRole] = "trusted";
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

        case IconRole:
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

        case TrustedRole:
            ret = device->isTrusted();
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

void DeviceFilter::filterOnType(QVector<Device::Type> types)
{
    m_types = types;
    m_typeEnabled = true;
    invalidateFilter();
}

void DeviceFilter::filterOnConnections(Device::Connections connections)
{
    m_connections = connections;
    m_connectionsEnabled = true;
    invalidateFilter();
}

void DeviceFilter::filterOnTrusted(bool trusted)
{
    m_trustedEnabled = true;
    m_trustedFilter = trusted;
    invalidateFilter();
}

bool DeviceFilter::filterAcceptsRow(int sourceRow,
                                    const QModelIndex &sourceParent) const
{
    bool accepts = true;
    QModelIndex childIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    if (accepts && m_typeEnabled) {
        const int type = childIndex.model()->data(childIndex, DeviceModel::TypeRole).value<int>();
        accepts = m_types.contains((Device::Type)type);
    }

    if (accepts && m_connectionsEnabled) {
        const int connection = childIndex.model()->data(childIndex, DeviceModel::ConnectionRole).value<int>();
        accepts = (m_connections & connection) != 0;
    }

    if (accepts && m_trustedEnabled) {
        const bool trusted = childIndex.model()->data(childIndex, DeviceModel::TrustedRole).value<bool>();
        accepts = trusted == m_trustedFilter;
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
