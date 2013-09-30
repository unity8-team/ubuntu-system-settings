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

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>

#include "agent.h"
#include "agentadaptor.h"
#include "devicemodel.h"
#include "killswitch.h"

class Bluetooth : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QAbstractItemModel* connectedHeadsets
                READ getConnectedHeadsets
                CONSTANT)

    Q_PROPERTY (QAbstractItemModel* disconnectedHeadsets
                READ getDisconnectedHeadsets
                CONSTANT)

    Q_PROPERTY (QObject * selectedDevice
                READ getSelectedDevice
                NOTIFY selectedDeviceChanged);

    Q_PROPERTY (QObject * agent
                READ getAgent);

    Q_PROPERTY (bool enabled
                READ isEnabled
                WRITE setEnabled
                NOTIFY enabledChanged);

    Q_PROPERTY (bool discovering
                READ isDiscovering
                NOTIFY discoveringChanged);

Q_SIGNALS:
    void selectedDeviceChanged();
    void enabledChanged(bool enabled);
    void discoveringChanged(bool isActive);

private Q_SLOTS:
    void onPairingDone();
    void onKillSwitchChanged(bool blocked);
    void slotDevicePairedChanged();

public:
    Bluetooth(QObject *parent = 0);
    ~Bluetooth() {}

    Q_INVOKABLE void setSelectedDevice(const QString &address);
    Q_INVOKABLE void connectHeadset(const QString &address);
    Q_INVOKABLE void disconnectHeadset();
    Q_INVOKABLE void connectAudioSource();
    Q_INVOKABLE void disconnectAudioSource();

public:
    Agent * getAgent();
    Device * getSelectedDevice();
    QAbstractItemModel * getConnectedHeadsets();
    QAbstractItemModel * getDisconnectedHeadsets();

    bool isEnabled() const { return !m_killswitch.isBlocked(); }
    void setEnabled(bool enabled) { m_killswitch.trySetBlocked(!enabled); }
    bool isDiscovering() const { return m_devices.isDiscovering(); }

private:
    QDBusConnection m_dbus;
    RfKillSwitch m_killswitch;
    DeviceModel m_devices;
    DeviceFilter m_connectedHeadsets;
    DeviceFilter m_disconnectedHeadsets;
    QSharedPointer<Device> m_selectedDevice;

    Agent m_agent;

    QMap<QString,Device::ConnectionMode> m_connectAfterPairing;
};

#endif // BLUETOOTH_H

