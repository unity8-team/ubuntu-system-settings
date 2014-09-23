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
#include <QDebug>
#include <unistd.h>
#include <QDBusMetaType>

typedef QList<QVariantMap> resetLauncherItemsArg;
Q_DECLARE_METATYPE(resetLauncherItemsArg)

Reset::Reset(QObject *parent)
    : QObject(parent)
{
    static bool isRegistered = false;
    if(!isRegistered) {
        qDBusRegisterMetaType<resetLauncherItemsArg>();
        isRegistered = true;
    }
}

bool Reset::resetLauncher()
{
    QList<QVariantMap> items;
    QVariantMap defaults;
    defaults.insert("defaults", true);
    items << defaults;
    QVariant answer = m_accountsService.setUserProperty(
                "com.canonical.unity.AccountsService",
                "launcher-items",
                QVariant::fromValue(items));

    if (answer.isValid())
        return true;

    return false;
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
