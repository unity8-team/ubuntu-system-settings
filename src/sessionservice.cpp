/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sessionservice.h"

#include <QDebug>

#define LM_SERVICE "org.freedesktop.login1"
#define LM_PATH "/org/freedesktop/login1"
#define LM_IFACE "org.freedesktop.login1.Manager"

SessionService::SessionService(QObject *parent)
    : QObject(parent),
      m_systemBusConnection(QDBusConnection::systemBus()),
      m_loginManager(LM_SERVICE,
                     LM_PATH,
                     LM_IFACE,
                     m_systemBusConnection)
{
}

void SessionService::reboot()
{
    m_loginManager.call("Reboot", false);
}
