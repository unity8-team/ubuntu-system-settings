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

void MockDisplays::connectDevice(const QString &address)
{
    Q_UNUSED(address)
}

void MockDisplays::disconnectDevice(const QString &address)
{
    Q_UNUSED(address)
}

void MockDisplays::scan()
{
    m_scanning = true;
    Q_EMIT scanningChanged(m_scanning);
}

void MockDisplays::stopScan()
{
    m_scanning = false;
    Q_EMIT scanningChanged(m_scanning);
}

void MockDisplays::setProperties(const QMap<QString, QVariant> &properties)
{
    Q_UNUSED(properties)
}

QAbstractItemModel* MockDisplays::devices()
{
    auto ret = &m_devices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

QAbstractItemModel* MockDisplays::connectedDevices()
{
    auto ret = &m_connectedDevices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

QAbstractItemModel* MockDisplays::disconnectedDevices()
{
    auto ret = &m_disconnectedDevices;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

bool MockDisplays::scanning() const
{
    return m_scanning;
}

bool MockDisplays::enabled() const
{
    return m_enabled;
}

void MockDisplays::setEnabled(const bool enabled)
{
    m_enabled = enabled;
    Q_EMIT enabledChanged(enabled);
}

QString MockDisplays::state() const
{
    return m_state;
}
