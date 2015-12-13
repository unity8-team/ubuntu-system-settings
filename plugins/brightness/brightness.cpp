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

#include <qpa/qplatformnativeinterface.h>

#include <QDBusArgument>
#include <QDBusReply>
#include <QDBusMetaType>
#include <QDebug>
#include <QGuiApplication>

static void connection_callback(MirConnection *new_connection, void *context)
{
    //((MirDemoState*)context)->connection = new_connection;
    qWarning() << "cb!";
}


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

typedef struct MirDemoState
{
    MirConnection *connection;
    MirSurface *surface;
} MirDemoState;

Brightness::Brightness(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_powerdIface ("com.canonical.powerd",
                   "/com/canonical/powerd",
                   "com.canonical.powerd",
                   m_systemBusConnection),
    m_powerdRunning(false),
    m_autoBrightnessAvailable(false)
{
    qRegisterMetaType<BrightnessParams>();
    m_powerdRunning = m_powerdIface.isValid();

    MirDemoState mcd;
    mcd.connection = 0;
    mcd.surface = 0;

    qWarning() << "Connecting...";
    mir_wait_for(mir_connect(nullptr, __FILE__, connection_callback, &mcd));
    qWarning() << "Connected!";

    if (mcd.connection == nullptr || !mir_connection_is_valid(mcd.connection))
    {
        const char *error = "Unknown error";
        if (mcd.connection != nullptr)
            error = mir_connection_get_error_message(mcd.connection);
        qWarning() << error;
        // fprintf(stderr, "Failed to connect to server `%s': %s\n",
        //         server == nullptr ? "<default>" : server, error);
        // return 1;
    }

    // auto *conn = static_cast<MirConnection*>(
    //         QGuiApplication::platformNativeInterface()->nativeResourceForIntegration(
    //                 "mirConnection"));
    // // MirDisplayConfiguration *conf = conn->create_copy_of_display_config()();
    // qWarning() << conn;
    // if (mir_connection_is_valid(conn)) {
    //     qWarning() << "mir connection is valid";
    // } else {
    //     qWarning() << "mir connection is NOT valid";
    //     mir_connection_get_error_message(conn);
    // }
    // MirDisplayConfiguration *conf = mir_connection_create_display_config(conn);
    // if (conf) {
    //     qWarning() << "num_cards" << conf->num_cards;
    // } else {
    //     qWarning() << "did not get a good mir display config";
    // }

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
}

bool Brightness::getAutoBrightnessAvailable() const
{
    return m_autoBrightnessAvailable;
}

bool Brightness::getPowerdRunning() const {
    return m_powerdRunning;
}
