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

#include "displays.h"

#include <QDebug>

Displays::Displays(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_unityInterface ("com.canonical.unity",
                   "/com/canonical/unity",
                   "com.canonical.unity",
                   m_systemBusConnection),
    m_displays()
{
}

Displays::~Displays()
{
}

QStringList Displays::displays() const
{
    // return m_displays;
    QStringList foo;
    foo << "MHL";
    return foo;
}
