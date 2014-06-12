/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Antti Kaijanmäki <antti.kaijanmaki@canonical.com>
 *
 */

#include "flight-mode-helper.h"

#include <iostream>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

FlightModeHelper::FlightModeHelper(QObject *parent)
    : QObject(parent)
{
   m_urfkill = new org::freedesktop::URfkill(QLatin1String("org.freedesktop.URfkill"),
                                             QLatin1String("/org/freedesktop/URfkill"),
                                             QDBusConnection::systemBus(),
                                             this);
    auto reply = m_urfkill->IsFlightMode();
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning("Failed to get flight-mode status: %s", qPrintable(reply.error().message()));
    }
    m_isFlightMode = reply.value();

    connect(m_urfkill, &org::freedesktop::URfkill::FlightModeChanged, [this](bool value)
    {
        m_isFlightMode = value;
        inFlightModeChanged();
    });
}

FlightModeHelper::~FlightModeHelper()
{}

void
FlightModeHelper::setFlightMode(bool value)
{
    m_urfkill->FlightMode(value);
}

bool
FlightModeHelper::inFlightMode()
{
    return m_isFlightMode;
}
