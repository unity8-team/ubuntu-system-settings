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

#include <QDBusMessage>
#include "brightness.h"

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

    QDBusMessage reply(m_powerdIface.call("getBrightnessParams"));

    if (reply.type() != QDBusMessage::ReplyMessage)
        return;

    // (iiib) -> max, min, default, autobrightness
    m_autoBrightnessAvailable = reply.arguments()[3].toBool();
}

bool Brightness::getPowerdRunning() const {
    return m_powerdRunning;
}
