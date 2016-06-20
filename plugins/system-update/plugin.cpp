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
#include "enums.h"
#include "systemimage.h"
#include "systemupdate.h"
#include "clickupdatemanager.h"
#include "updatemodel.h"

using namespace UpdatePlugin;

static QObject *suSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return SystemUpdate::instance();
}

static QObject *siSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new QSystemImage;
}

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.Update"));

    qmlRegisterSingletonType<SystemUpdate>(
        uri, 1, 0, "SystemUpdate", suSingletonProvider
    );
    // SI is a singleton because we use it both in the EntryComponent as well
    // as in the PageComponent.
    qmlRegisterSingletonType<QSystemImage>(
        uri, 1, 0, "SystemImage", siSingletonProvider
    );

    qmlRegisterUncreatableType<Enums>(
        uri, 1, 0, "Enums", "Enums can't be instantiated directly."
    );

    qmlRegisterType<ClickUpdateManager>(uri, 1, 0, "ClickUpdateManager");
    qmlRegisterType<UpdateModel>(uri, 1, 0, "UpdateModel");
}
