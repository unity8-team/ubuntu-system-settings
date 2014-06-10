/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Antti Kaijanmäki <antti.kaijanmaki@canonical.com>
 *
 */

#include <QtQml>
#include <QtQml/QQmlContext>
#include "plugin.h"

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.FlightMode"));
    
//    qmlRegisterType<Bluetooth>(uri, 1, 0, "UbuntuBluetoothPanel");
//    qmlRegisterType<Device>(uri, 1, 0, "Device");
//    qmlRegisterType<Device>(uri, 1, 0, "Agent");
//    qRegisterMetaType<Device*>("Device*");
//    qRegisterMetaType<Agent*>("Agent*");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
