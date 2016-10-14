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

#ifndef MOCK_AETHERCAST_DEVICE_H
#define MOCK_AETHERCAST_DEVICE_H

#include <QObject>

class MockAethercastDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString Address READ address)
    Q_PROPERTY(QStringList Capabilities READ capabilities)
    Q_PROPERTY(QString Name READ name)
    Q_PROPERTY(State state READ state)
public:
    MockAethercastDevice(QObject *parent = 0) {
        Q_UNUSED(parent)
    };
    ~MockAethercastDevice() {};

    enum State { Idle=1, Disconnected=2, Association=4, Configuration=8, Connected=16, Failure=32 };
    Q_ENUMS(State)
    Q_DECLARE_FLAGS(States, State)

    inline QString address() const
    { return m_address; }

    inline QStringList capabilities() const
    { return m_capabilities; }

    inline QString name() const
    { return m_name; }

    inline State state() const
    { return m_state; }

    QString m_address = QString::null;
    QStringList m_capabilities;
    QString m_name = QString::null;
    State m_state = MockAethercastDevice::Idle;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MockAethercastDevice::States)

#endif // MOCK_AETHERCAST_DEVICE_H
