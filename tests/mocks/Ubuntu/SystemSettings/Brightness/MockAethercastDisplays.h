/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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

#ifndef MOCK_AETHERCAST_DISPLAYS_H
#define MOCK_AETHERCAST_DISPLAYS_H

#include "MockAethercastDeviceModel.h"

#include <QObject>

class MockDisplays : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QAbstractItemModel* devices
                READ devices
                CONSTANT)
    Q_PROPERTY (QAbstractItemModel* connectedDevices
                READ connectedDevices
                NOTIFY connectedDevicesChanged)
    Q_PROPERTY (QAbstractItemModel* disconnectedDevices
                READ disconnectedDevices
                NOTIFY disconnectedDevicesChanged)
    Q_PROPERTY (bool scanning
                READ scanning
                NOTIFY scanningChanged)
    Q_PROPERTY (bool enabled
                READ enabled
                WRITE setEnabled
                NOTIFY enabledChanged)
    Q_PROPERTY (QString state
                READ state
                NOTIFY stateChanged)
public:
    explicit MockDisplays(QObject *parent = nullptr) {
        Q_UNUSED(parent)
    };
    ~MockDisplays() {}

    enum Error {
        None,
        Failed,
        Already,
        ParamInvalid,
        InvalidState,
        NotConnected,
        NotReady,
        Unknown
    };
    Q_ENUMS(Error)

    Q_INVOKABLE void connectDevice(const QString &address);
    Q_INVOKABLE void disconnectDevice(const QString &address);
    Q_INVOKABLE void scan();
    Q_INVOKABLE void stopScan(); // mock only
    Q_INVOKABLE void setEnabled(const bool enabled);
    void setProperties(const QMap<QString, QVariant> &properties);
    QAbstractItemModel * devices();
    QAbstractItemModel * connectedDevices();
    QAbstractItemModel * disconnectedDevices();
    bool scanning() const;
    bool enabled() const;
    QString state() const;
    MockAethercastDeviceModel m_devices;
    MockAethercastDeviceModel m_connectedDevices;
    MockAethercastDeviceModel m_disconnectedDevices;
    QString m_state = QString::null;

private:
    bool m_scanning = false;
    bool m_enabled = false;

Q_SIGNALS:
    void scanningChanged(bool isActive);
    void enabledChanged(bool enabled);
    void stateChanged();
    void connectedDevicesChanged();
    void disconnectedDevicesChanged();
    void connectError(int error);
};

#endif // MOCK_AETHERCAST_DISPLAYS_H
