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
#include <QDBusMetaType>
#include "brightness.h"

// Returned data from getBrightnessParams
struct BrightnessParams {
        int dim; // Dim brightness
        int min; // Minimum brightness
        int max; // Maximum brightness
        int def; // Default brightness
        bool automatic; // Whether "auto brightness" is supported
};
Q_DECLARE_METATYPE(BrightnessParams)

const QDBusArgument &operator<<(QDBusArgument &argument,
                                const BrightnessParams &params)
{
    argument.beginStructure();
    argument << params.dim << params.min << params.max << params.def << params.automatic;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                BrightnessParams &params)
{
    argument.beginStructure();
    argument >> params.dim >> params.min >> params.max >> params.def >> params.automatic;
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
    qRegisterMetaType<BrightnessParams>();
    m_powerdRunning = m_powerdIface.isValid();

    if (!m_powerdRunning)
        return;

    QDBusMessage reply(m_powerdIface.call("getBrightnessParams"));

    if (reply.type() != QDBusMessage::ReplyMessage)
        return;

    // (iiiib) -> dim, max, min, default, autobrightness
    QDBusArgument result(reply.arguments()[0].value<QDBusArgument>());
    BrightnessParams params = qdbus_cast<BrightnessParams>(result);
    m_autoBrightnessAvailable = params.automatic;
}

bool Brightness::getAutoBrightnessAvailable() const
{
    return m_autoBrightnessAvailable;
}

bool Brightness::getPowerdRunning() const {
    return m_powerdRunning;
}
