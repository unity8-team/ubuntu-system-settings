/*
 * Copyright (C) 2015 Canonical Ltd
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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
 */

#include <QDebug>

#include "displayserver.h"

typedef struct MirState
{
    MirConnection *connection;
} MirState;

static void mir_connection_callback(MirConnection *new_connection, void *context)
{
    qWarning() << "Connection callback.";
    ((MirState*)context)->connection = new_connection;
}


DisplayServerConnection get_display_server_connection() {
    DisplayServerConnection dsc;
    dsc.connection = get_mir_display_server_connection();
    return dsc;
}

MirConnection *get_mir_display_server_connection() {
    MirState mirState;
    mirState.connection = 0;

    qWarning() << "Connecting...";
    mir_wait_for(mir_connect(nullptr, __FILE__, mir_connection_callback, &mirState));
    qWarning() << "Connected!";

    if (mirState.connection == nullptr || !mir_connection_is_valid(mirState.connection)) {
        const char *error = "Unknown error";
        if (mirState.connection != nullptr)
            error = mir_connection_get_error_message(mirState.connection);
        qWarning() << error;
    } else {
        qWarning() << "Using mir as display server.";
        return mirState.connection;
    }
    return nullptr;
}
