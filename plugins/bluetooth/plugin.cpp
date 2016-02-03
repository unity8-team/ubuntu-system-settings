/*
 * Copyright (C) 2013-2015 Canonical Ltd
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

#include "plugin.h"

#include <QtQml>
#include <QtQml/QQmlContext>
#include "bluetooth.h"
#include "device.h"
#include "bluez_helper.h"

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.Bluetooth"));

    // Register additional QtDBus types we need
    qDBusRegisterMetaType<InterfaceList>();
    qDBusRegisterMetaType<ManagedObjectList>();

    // .. now register our real QML types
    qmlRegisterType<Bluetooth>(uri, 1, 0, "UbuntuBluetoothPanel");
    qmlRegisterType<Device>(uri, 1, 0, "Device");
    qmlRegisterType<Device>(uri, 1, 0, "Agent");
    qRegisterMetaType<Device*>("Device*");
    qRegisterMetaType<Agent*>("Agent*");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
