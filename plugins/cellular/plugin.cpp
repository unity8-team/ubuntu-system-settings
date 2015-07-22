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
*/

#include "plugin.h"

#include <QtQml>
#include <QtQml/QQmlContext>
#include "connectivity.h"
#include "ofonoactivator.h"

static QObject *connectivitySingeltonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    Connectivity *connectivity = new Connectivity();
    return connectivity;
}

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.Cellular"));
    qmlRegisterSingletonType<Connectivity>(uri, 1, 0, "Connectivity", connectivitySingeltonProvider);
    qmlRegisterType<OfonoActivator>(uri, 1, 0, "OfonoActivator");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
