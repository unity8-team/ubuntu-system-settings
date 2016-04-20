/*
 * Copyright (C) 2013-2014 Canonical Ltd
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
 * Didier Roche <didier.roche@canonical.com>
 *
*/

#include "plugin.h"

#include <QtQml>
#include <QtQml/QQmlContext>
#include "systemimage.h"
#include "updatemanager.h"

using namespace UpdatePlugin;

static QObject *umSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return UpdateManager::instance();
}

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.Update"));

    qmlRegisterUncreatableType<QSystemImage>(uri, 1, 0, "SystemImage",
        "System Image can't be instantiated directly.");

    // The context is already populated with “UpdateManager”, but we still need to
    // register it as a type.
    // TODO: Register as well as providing a singleton via qmlRegisterSingleton.
    // qmlRegisterUncreatableType<UpdateManager>(uri, 1, 0, "UM", "");
    qmlRegisterSingletonType<UpdateManager>(uri, 1, 0, "UpdateManager", umSingletonProvider);
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
    // QQmlContext* context = engine->rootContext();
    // context->setContextProperty("UpdateManager",
    //                             UpdateManager::instance());
}
