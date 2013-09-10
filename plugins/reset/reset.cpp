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
#include <QEvent>
#include <QDBusReply>
#include <unistd.h>
#include <QtCore/QDebug>

Reset::Reset(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_accountsserviceIface ("org.freedesktop.Accounts",
                            "/org/freedesktop/Accounts",
                            "org.freedesktop.Accounts",
                             m_systemBusConnection)
{
    if (!m_accountsserviceIface.isValid()) {
        return;
    }

    QDBusReply<QDBusObjectPath> qObjectPath = m_accountsserviceIface.call(
                "FindUserById", qlonglong(getuid()));

    if (qObjectPath.isValid()) {
        m_objectPath = qObjectPath.value().path();
    }
}

bool Reset::resetLauncher()
{
    QDBusInterface userInterface (
                "org.freedesktop.Accounts",
                m_objectPath,
                "org.freedesktop.DBus.Properties.Set",
                m_systemBusConnection,
                this);

    if (!userInterface.isValid())
        return false;

    QList<QVariantMap> items;
    QVariantMap defaults;
    defaults.insert("defaults", true);
    items << defaults;
    /* TODO: test again-enable once the unity side lands
        userInterface.call("Set",
                       "com.canonical.unity.AccountsService",
                       "launcher-items",
                       QVariant::fromValue(items));*/
    return true;
}

Reset::~Reset() {
}
