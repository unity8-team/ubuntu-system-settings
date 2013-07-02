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

#include "background.h"
#include <QDebug>
#include <QEvent>
#include <QDBusReply>
#include <unistd.h>

Background::Background(QObject *parent) :
    QObject(parent),
    system_bus_connection (QDBusConnection::systemBus()),
    accountsservice_iface ("org.freedesktop.Accounts",
                           "/org/freedesktop/Accounts",
                           "org.freedesktop.Accounts",
                            system_bus_connection)
{
    if (!accountsservice_iface.isValid()) {
        return;
    }

    background_file = get_background_file();

    QDBusReply<QDBusObjectPath> q_object_path = accountsservice_iface.call(
                "FindUserById", qlonglong(getuid()));

    if (q_object_path.isValid()) {
        object_path = q_object_path.value().path();
    }

    system_bus_connection.connect("org.freedesktop.Accounts",
                                  object_path,
                                  "org.freedesktop.Accounts.User",
                                  "Changed",
                                  this,
                                  SLOT(slotChanged()));

    background_file = get_background_file();
}

QString Background::get_background_file()
{
    QDBusInterface user_interface (
                "org.freedesktop.Accounts",
                object_path,
                "org.freedesktop.Accounts.User",
                system_bus_connection,
                this);

    if (user_interface.isValid()) {
        return user_interface.property("BackgroundFile").toString();
    }

    return QString();
}

void Background::slotChanged()
{
    QString new_background = get_background_file();
    if (new_background != background_file) {
        qDebug() << "Background changed to: " << new_background;
        background_file = new_background;
        Q_EMIT backgroundFileChanged();
    }
}

QString Background::backgroundFile()
{
     return background_file;
}

Background::~Background() {
}
