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

#ifndef USS_BLUETOOTH_AGENT_H
#define USS_BLUETOOTH_AGENT_H

#include <QObject>
#include <QMap>
#include <QSharedPointer>

#include <QtDBus>

#include "device.h"
#include "devicemodel.h"

class Agent: public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    Agent(QDBusConnection connection, DeviceModel &devices, QObject *parent=0):
        QObject(parent), m_connection(connection), m_devices(devices) {}
    virtual ~Agent() {}
    Q_INVOKABLE void confirmPasskey(uint tag, bool confirmed);
    Q_INVOKABLE void providePasskey(uint tag, bool provided, uint passkey);

public Q_SLOTS: // received from the system's bluez service
    void Cancel();
    void DisplayPasskey(const QDBusObjectPath &path, uint passkey, uchar entered);
    void Release();
    void RequestConfirmation(const QDBusObjectPath &path, uint passkey);
    uint RequestPasskey(const QDBusObjectPath &path);
    QString RequestPinCode(const QDBusObjectPath &path);

Q_SIGNALS:
    void passkeyNeeded(int tag, Device* device);
    void passkeyConfirmationNeeded(int tag, Device* device, QString passkey);
    void onPairingDone();

private:
    Q_DISABLE_COPY(Agent);

    QDBusConnection m_connection;
    DeviceModel &m_devices;
    QMap<uint,QDBusMessage> m_delayedReplies;
    uint m_tag = 1;
};

Q_DECLARE_METATYPE(Agent*)


#endif // USS_BLUETOOTH_AGENT_H
