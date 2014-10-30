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
 * Sebastien Bacher <sebastien.bacher@canonical.com>
 *
*/

#include "reset.h"

#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

Reset::Reset(QObject *parent)
    : QObject(parent)
{
}

bool Reset::factoryReset()
{
    QDBusInterface iface (
                "com.canonical.SystemImage",
                "/Service",
                "com.canonical.SystemImage",
                QDBusConnection::systemBus(),
                this);

    // FIXME: temporary warning so we know we've reproduced bug #1370815
    if (!iface.isValid())
        qWarning() << iface.interface() << "Isn't valid";


    QDBusReply<QString> reply = iface.call("FactoryReset");
    if (!reply.isValid()) {
         qWarning() << reply.error().message(); 
        return false;
    }

    return true;
}

Reset::~Reset() {
}
