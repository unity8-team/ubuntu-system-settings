/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *    Jonas G. Drange <jonas.drange@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDBusReply>
#include <QtDebug>
#include <QDBusInterface>
#include "connectivity.h"

namespace {
    const QString conn_service("com.ubuntu.connectivity1");
    const QString conn_object("/com/ubuntu/connectivity1/Private");
    const QString conn_interface("com.ubuntu.connectivity1.Private");
    const QString conn_unlockall_method("UnlockAllModems");
}

Connectivity::Connectivity(QObject *parent) : QObject(parent)
{
}

void Connectivity::unlockAllModems() {

    QDBusInterface connectivityIface (
        conn_service,
        conn_object,
        conn_interface,
        QDBusConnection::sessionBus(),
        this);

    auto reply = connectivityIface.call(conn_unlockall_method);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Failed to unlock modems" << reply.errorMessage();
    }
}
