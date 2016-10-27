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

#include "brightness.h"
#include "displays/mirdisplays_impl.h"
#include <hybris/properties/properties.h>

#include <QDBusArgument>
#include <QDBusReply>
#include <QDBusMetaType>
#include <QDebug>
#include <QQmlEngine>

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

Brightness::Brightness(QDBusConnection dbus, MirDisplays *mirDisplays,
                       QObject *parent)
    : QObject(parent)
    , m_systemBusConnection(dbus)
    , m_mirDisplays(mirDisplays)
    , m_powerdIface("com.canonical.powerd",
                    "/com/canonical/powerd",
                    "com.canonical.powerd",
                    m_systemBusConnection)
    , m_powerdRunning(false)
    , m_autoBrightnessAvailable(false)

{
    qRegisterMetaType<BrightnessParams>();
    m_powerdRunning = m_powerdIface.isValid();

    if (!m_powerdRunning) {
        qWarning() << m_powerdIface.interface() << m_powerdIface.lastError().message();
        return;
    }

    QDBusMessage reply(m_powerdIface.call("getBrightnessParams"));

    if (reply.type() != QDBusMessage::ReplyMessage)
        return;

    // (iiiib) -> dim, max, min, default, autobrightness
    QDBusArgument result(reply.arguments()[0].value<QDBusArgument>());
    BrightnessParams params = qdbus_cast<BrightnessParams>(result);
    m_autoBrightnessAvailable = params.automatic;
    m_changedDisplays.filterOnUncommittedChanges(true);
    m_changedDisplays.setSourceModel(&m_displays);
}

Brightness::Brightness(QObject *parent) :
    Brightness(QDBusConnection::systemBus(), new MirDisplaysImpl(), parent)
{
}

bool Brightness::getAutoBrightnessAvailable() const
{
    return m_autoBrightnessAvailable;
}

bool Brightness::getPowerdRunning() const {
    return m_powerdRunning;
}

bool Brightness::getWidiSupported() const
{
    char widi[PROP_VALUE_MAX];
    property_get("ubuntu.widi.supported", widi, "0");
    return (strcmp(widi, "0") > 0);
}

QAbstractItemModel* Brightness::allDisplays()
{
    auto ret = &m_displays;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

QAbstractItemModel* Brightness::changedDisplays()
{
    auto ret = &m_changedDisplays;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}
