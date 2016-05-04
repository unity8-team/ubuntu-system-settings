/*
 * Copyright (C) 2013-2015 Canonical Ltd
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

#include "devicemodel.h"

#include <QDBusReply>
#include <QDebug>
#include <QTime>
#include <QCoreApplication>

#include "dbus-shared.h"

namespace
{
  const int SCANNING_ACTIVE_DURATION_MSEC = (30 * 1000);
  const int SCANNING_IDLE_DURATION_MSEC = (10 * 1000);
}

DeviceModel::DeviceModel(QDBusConnection &dbus, QObject *parent):
    QAbstractListModel(parent),
    m_dbus(dbus),
    m_bluezManager("org.bluez", "/", m_dbus),
    m_bluezAgentManager("org.bluez", "/org/bluez", m_dbus),
    m_isPowered(false),
    m_isPairable(false),
    m_isDiscovering(false),
    m_isDiscoverable(false),
    m_discoveryBlockCount(0),
    m_activeDevices(0)
{
    if (m_bluezManager.isValid()) {

        connect(&m_bluezManager, SIGNAL(InterfacesAdded(const QDBusObjectPath&, InterfaceList)),
                this, SLOT(slotInterfacesAdded(const QDBusObjectPath&, InterfaceList)));

        connect(&m_bluezManager, SIGNAL(InterfacesRemoved(const QDBusObjectPath&, const QStringList&)),
                this, SLOT(slotInterfacesRemoved(const QDBusObjectPath&, const QStringList&)));

        watchCall(m_bluezManager.GetManagedObjects(), [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<ManagedObjectList> reply = *watcher;

            if (reply.isError()) {
                qWarning() << "Failed to retrieve list of managed objects from BlueZ service: "
                           << reply.error().message();
                watcher->deleteLater();
                return;
            }

            auto objectList = reply.argumentAt<0>();

            for (QDBusObjectPath path : objectList.keys()) {
                InterfaceList ifaces = objectList.value(path);

                if (!ifaces.contains(BLUEZ_ADAPTER_IFACE))
                    continue;

                // Ok, here we've found an adapter. As we don't expect multiple at the
                // moment we just take the first one we find.
                setAdapterFromPath(path.path(), ifaces.value(BLUEZ_ADAPTER_IFACE));
                break;
            }

            watcher->deleteLater();
        });
    }

    if (m_bluezAgentManager.isValid()) {
        // NOTE: We can safely register our agent here even if we don't
        // manage any adapter yet. BlueZ makes sure our agent will only
        // process requests related to actions we do unless we our agent
        // the system default one.
        auto call = m_bluezAgentManager.RegisterAgent(QDBusObjectPath(DBUS_ADAPTER_AGENT_PATH),
                                                      DBUS_AGENT_CAPABILITY);

        watchCall(call, [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<void> reply = *watcher;

            if (reply.isError()) {
                qWarning() << "Failed to register our agent with BlueZ:"
                           << reply.error().message();
            }
            else {
                setupAsDefaultAgent();
            }

            watcher->deleteLater();
        });
    }
    else {
        qWarning() << "Could not register agent with BlueZ service as "
                   << "the agent manager is not available!";
    }

    connect(&m_discoveryTimer, SIGNAL(timeout()), this, SLOT(slotDiscoveryTimeout()));
}

DeviceModel::~DeviceModel()
{
    clearAdapter();

    qWarning() << "Releasing device model ..";

    if (m_bluezAgentManager.isValid()) {
        auto call = m_bluezAgentManager.UnregisterAgent(QDBusObjectPath(DBUS_ADAPTER_AGENT_PATH));
        watchCall(call, [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<void> reply = *watcher;

            if (reply.isError()) {
                qWarning() << "Failed to unregister our agent with BlueZ:"
                           << reply.error().message();
            }

            watcher->deleteLater();
        });
    }
}

void DeviceModel::setupAsDefaultAgent()
{
    auto call = m_bluezAgentManager.RequestDefaultAgent(QDBusObjectPath(DBUS_ADAPTER_AGENT_PATH));
    watchCall(call, [=](QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<void> reply = *watcher;

        if (reply.isError()) {
            qWarning() << "Failed to setup ourself as default agent: "
                       << reply.error().message();
        }

        watcher->deleteLater();
    });
}

void DeviceModel::slotInterfacesAdded(const QDBusObjectPath &objectPath, InterfaceList ifacesAndProps)
{
    Q_UNUSED(ifacesAndProps);

    auto candidatedPath = objectPath.path();

    if (!m_bluezAdapter) {
        // Maybe we have a new adapter we can start to use?
        if (ifacesAndProps.contains(BLUEZ_ADAPTER_IFACE))
            setAdapterFromPath(candidatedPath, ifacesAndProps.value(BLUEZ_ADAPTER_IFACE));

        return;
    }

    // At this point we can only get new devices
    if (!candidatedPath.startsWith(m_bluezAdapter->path()))
        return;

    if (!ifacesAndProps.contains(BLUEZ_DEVICE_IFACE))
        return;

    addDevice(candidatedPath, ifacesAndProps.value(BLUEZ_DEVICE_IFACE));
}

void DeviceModel::slotInterfacesRemoved(const QDBusObjectPath &objectPath, const QStringList &interfaces)
{
    auto candidatedPath = objectPath.path();

    if (!m_bluezAdapter)
        return;

    if (candidatedPath == m_bluezAdapter->path() &&
        interfaces.contains(BLUEZ_ADAPTER_IFACE)) {
        clearAdapter();
        return;
    }

    if (!candidatedPath.startsWith(m_bluezAdapter->path()))
        return;

    if (!interfaces.contains(BLUEZ_DEVICE_IFACE))
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

void DeviceModel::restartDiscoveryTimer()
{
    if (m_discoveryBlockCount > 0)
        return;

    m_discoveryTimer.start (m_isDiscovering ? SCANNING_ACTIVE_DURATION_MSEC
                                   : SCANNING_IDLE_DURATION_MSEC);
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
    if (m_bluezAdapter && m_isPowered && m_isDiscovering) {

         watchCall(m_bluezAdapter->StopDiscovery(), [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<void> reply = *watcher;
            if (reply.isError()) {
                qWarning() << "Failed to stop device discovery:"
                           << reply.error().message();
            }

            watcher->deleteLater();
        });
    }
}

void DeviceModel::startDiscovery()
{
    if (m_bluezAdapter && m_isPowered && !m_isDiscovering) {

        watchCall(m_bluezAdapter->StartDiscovery(), [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<void> reply = *watcher;
            if (reply.isError()) {
                qWarning() << "Failed to start device discovery:"
                           << reply.error().message();
            }

            watcher->deleteLater();
        });
    }
}

void DeviceModel::toggleDiscovery()
{
    if (isDiscovering())
        stopDiscovery();
    else
        startDiscovery();
}

void DeviceModel::slotDiscoveryTimeout()
{
    toggleDiscovery();
}

void DeviceModel::clearAdapter()
{
    if (m_bluezAdapter) {

        stopDiscovery();
        m_discoverableTimer.stop();
        trySetDiscoverable(false);

        m_bluezAdapter.reset(0);
        m_bluezAdapterProperties.reset(0);
        m_adapterName.clear();

        beginResetModel();
        m_devices.clear();
        endResetModel();
    }
}

void DeviceModel::setAdapterFromPath(const QString &path, const QVariantMap &properties)
{
    clearAdapter();

    if (!path.isEmpty()) {

        auto adapter = new BluezAdapter1(BLUEZ_SERVICE, path, m_dbus);
        auto adapterProperties = new FreeDesktopProperties(BLUEZ_SERVICE, path, m_dbus);

        m_bluezAdapter.reset(adapter);
        m_bluezAdapterProperties.reset(adapterProperties);

        startDiscovery();
        updateDevices();

        setProperties(properties);

        connect(adapterProperties, SIGNAL(PropertiesChanged(const QString&, const QVariantMap&, const QStringList&)),
                this, SLOT(slotAdapterPropertiesChanged(const QString&, const QVariantMap&, const QStringList&)));

        // Delay enabling discoverability by 1 second.
        m_discoverableTimer.setSingleShot(true);
        connect(&m_discoverableTimer, SIGNAL(timeout()), this, SLOT(slotEnableDiscoverable()));
        m_discoverableTimer.start(1000);
    }
}

void DeviceModel::slotAdapterPropertiesChanged(const QString &interface, const QVariantMap &changedProperties,
                                               const QStringList &invalidatedProperties)
{
    Q_UNUSED(invalidatedProperties);

    if (interface != BLUEZ_ADAPTER_IFACE)
        return;

    setProperties(changedProperties);
}

void DeviceModel::updateDevices()
{
    watchCall(m_bluezManager.GetManagedObjects(), [=](QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<ManagedObjectList> reply = *watcher;

        if (reply.isError()) {
            qWarning() << "Failed to retrieve list of managed objects from BlueZ service: "
                       << reply.error().message();
            watcher->deleteLater();
            return;
        }

        auto objectList = reply.argumentAt<0>();

        for (auto objectPath : objectList.keys()) {
            auto candidatePath = objectPath.path();

            if (!candidatePath.startsWith(m_bluezAdapter->path()))
                continue;

            InterfaceList ifaces = objectList.value(objectPath);

            if (!ifaces.contains(BLUEZ_DEVICE_IFACE))
                continue;

            auto properties = ifaces.value(BLUEZ_DEVICE_IFACE);

            addDevice(candidatePath, properties);
        }

    });
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
        Q_EMIT(adapterNameChanged());
    } else if (key == "Address") {
        m_adapterAddress = value.toString();
        Q_EMIT(adapterAddressChanged());
    } else if (key == "Pairable") {
        m_isPairable = value.toBool();
    } else if (key == "Discoverable") {
        setDiscoverable(value.toBool());
    } else if (key == "Discovering") {
        setDiscovering(value.toBool());
        restartDiscoveryTimer();
    } else if (key == "Powered") {
        setPowered(value.toBool());
        if (m_isPowered)
            trySetDiscoverable(true);
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
    if (m_isDiscoverable)
        return;

    QVariant value;
    QDBusVariant disc(discoverable);
    QDBusReply<void > reply;

    value.setValue(disc);

    if (m_bluezAdapter && m_bluezAdapter->isValid() && m_isPowered) {
        reply = m_bluezAdapterProperties->call("Set", BLUEZ_ADAPTER_IFACE, "Discoverable", value);
        if (!reply.isValid())
            qWarning() << "Error setting device discoverable:" << reply.error();
    }
}

void DeviceModel::blockDiscovery()
{
    m_discoveryBlockCount++;

    stopDiscovery();
    m_discoveryTimer.stop();
}

void DeviceModel::unblockDiscovery()
{
    if (m_discoveryBlockCount == 0)
        return;

    m_discoveryBlockCount--;

    if (m_discoveryBlockCount > 0)
        return;

    restartDiscoveryTimer();
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

void DeviceModel::slotDeviceConnectionChanged()
{
    Device *device = static_cast<Device*>(sender());

    // We count the number of active devices here (either disconnecting
    // or connecting) and will stop device discovery while those actions
    // are ongoing.

    unsigned int wasInactive = (m_activeDevices == 0);

    switch (device->getConnection()) {
    case Device::Disconnected:
    case Device::Connected:
        if (m_activeDevices == 0)
            break;

        m_activeDevices--;
        break;
    case Device::Disconnecting:
    case Device::Connecting:
        m_activeDevices++;
        break;
    default:
        break;
    }

    if (wasInactive && m_activeDevices > 0)
        blockDiscovery();
    else
        unblockDiscovery();
}

QSharedPointer<Device> DeviceModel::addDevice(const QString &path, const QVariantMap &properties)
{
    QSharedPointer<Device> device(new Device(path, m_dbus));
    device->setProperties(properties);

    // At this point the device is not valid oly when the device type has
    // not been updated to the value from the properties. Also we know that
    // when this function is called from FindOrCreateDevice() context the
    // properties are not yet known. In that case the device will become
    // valid once the device properties are gathered and this done
    // asynchronously in the Device class constructor which takes a while.
    // This is a problem in certain situations and as a consequence the
    // pin code request is rejected and the bonding denied. To workaround
    // this unwanted behavior we assume that at this point it is just a
    // matter of time for the device to beome valid and we wait a bit here.
    // This is safe because in any other case the properties are already
    // updated.
    uint8_t tries = 0;
    while (!device->isValid() && tries < 10) {
        QTime timeout = QTime::currentTime().addMSecs(100);
        while (QTime::currentTime() < timeout)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        tries++;
    }

    // However if the above fails there is nothing else that can be done
    // than returning the nullptr.
    if (!device->isValid())
        return QSharedPointer<Device>(nullptr);

    QObject::connect(device.data(), SIGNAL(deviceChanged()),
                     this, SLOT(slotDeviceChanged()));
    QObject::connect(device.data(), SIGNAL(pairingDone(bool)),
                     this, SLOT(slotDevicePairingDone(bool)));
    QObject::connect(device.data(), SIGNAL(connectionChanged()),
                     this, SLOT(slotDeviceConnectionChanged()));

    return addDevice(device);
}

QSharedPointer<Device> DeviceModel::addDevice(QSharedPointer<Device> &device)
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

    return device;
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

QSharedPointer<Device> DeviceModel::addDeviceFromPath(const QDBusObjectPath &path)
{
    qWarning() << "Creating device object for path" << path.path();
    QVariantMap noProps;
    return addDevice(path.path(), noProps);
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
    if (!m_bluezAdapter) {
        qWarning() << "Default adapter is not available for device removal";
        return;
    }

    QDBusPendingCall call = m_bluezAdapter->RemoveDevice(QDBusObjectPath(path));

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(slotRemoveFinished(QDBusPendingCallWatcher*)));
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
        QString displayName;

        switch (role) {
        case Qt::DisplayRole:
            displayName = device->getName();

            if (displayName.isEmpty())
                displayName = device->getAddress();

            if (!device->isPaired())
                displayName.append("…");

            ret = displayName;
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
