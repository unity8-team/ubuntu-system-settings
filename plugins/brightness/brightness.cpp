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
 * Iain Lane <iain.lane@canonical.com>
 *
 */

#include <QDBusArgument>
#include <QDBusReply>
#include "brightness.h"

struct Params {
        int min;
        int max;
        int def;
        bool automatic;
};
Q_DECLARE_METATYPE(Params)

const QDBusArgument &operator<<(QDBusArgument &argument, const Params &params)
{
    argument.beginStructure();
    argument << params.min << params.max << params.def << params.automatic;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Params &params)
{
    argument.beginStructure();
    argument >> params.min >> params.max >> params.def >> params.automatic;
    argument.endStructure();
    return argument;
}

Brightness::Brightness(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_powerdIface ("com.canonical.powerd",
                   "/com/canonical/powerd",
                   "com.canonical.powerd",
                   m_systemBusConnection)
{
    m_powerdRunning = m_powerdIface.isValid();

    if (!m_powerdRunning)
        return;

    QDBusReply<Params> reply(m_powerdIface.call("getBrightnessParams"));

    if (!reply.isValid())
        return;

    // (iiib) -> max, min, default, autobrightness
    Params result(reply.value());
    m_autoBrightnessAvailable = result.automatic;
}

bool Brightness::getAutoBrightnessAvailable() const
{
    return m_autoBrightnessAvailable;
}

bool Brightness::getPowerdRunning() const {
    return m_powerdRunning;
}
