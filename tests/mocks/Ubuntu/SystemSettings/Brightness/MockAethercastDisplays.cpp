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
#include "MockAethercastDisplays.h"

#include <QQmlEngine>

void MockAethercastDisplays::connectDevice(const QString &address)
{
    Q_UNUSED(address)
}

void MockAethercastDisplays::disconnectDevice(const QString &address)
{
    Q_UNUSED(address)
}

void MockAethercastDisplays::scan()
{
    m_scanning = true;
    Q_EMIT scanningChanged(m_scanning);
}

void MockAethercastDisplays::stopScan()
{
    m_scanning = false;
    Q_EMIT scanningChanged(m_scanning);
}

void MockAethercastDisplays::setProperties(const QMap<QString, QVariant> &properties)
{
    Q_UNUSED(properties)
}

QAbstractItemModel* MockAethercastDisplays::devices()
{
    auto ret = &m_devices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

QAbstractItemModel* MockAethercastDisplays::connectedDevices()
{
    auto ret = &m_connectedDevices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

QAbstractItemModel* MockAethercastDisplays::disconnectedDevices()
{
    auto ret = &m_disconnectedDevices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

bool MockAethercastDisplays::scanning() const
{
    return m_scanning;
}

bool MockAethercastDisplays::enabled() const
{
    return m_enabled;
}

void MockAethercastDisplays::setEnabled(const bool enabled)
{
    m_enabled = enabled;
    Q_EMIT enabledChanged(enabled);
}

QString MockAethercastDisplays::state() const
{
    return m_state;
}
