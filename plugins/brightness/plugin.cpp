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

#include "plugin.h"

#include <QtQml>
#include <QtQml/QQmlContext>
#include "brightness.h"
#include "aethercast/aethercast_helper.h"
#include "aethercast/displays.h"
#include "aethercast/device.h"

#include "displays/display.h"
#include "displays/displaymodel.h"


void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.Brightness"));

    // Register additional QtDBus types we need
    qDBusRegisterMetaType<InterfaceList>();
    qDBusRegisterMetaType<ManagedObjectList>();

    qmlRegisterType<Brightness>(uri, 1, 0, "UbuntuBrightnessPanel");
    qmlRegisterType<Device>(uri, 1, 0, "AethercastDevice");
    qmlRegisterType<Displays>(uri, 1, 0, "AethercastDisplays");

    qmlRegisterType<DisplayPlugin::DisplayModel>(uri, 1, 0, "DisplayModel");
    qmlRegisterUncreatableType<DisplayPlugin::Display>(uri, 1, 0, "Display", "Use UbuntuBrightnessPanel.");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
