/*
 * Copyright (C) 2013-2016 Canonical Ltd
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
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/

#include "click/manager.h"

#include "plugin.h"
#include "systemupdate.h"
#include "update.h"
#include "updatemodel.h"

#include <QtQml>
#include <QtQml/QQmlContext>

using namespace UpdatePlugin;

static QObject *suSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return SystemUpdate::instance();
}

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.Update"));

    qmlRegisterUncreatableType<Update>(
        uri, 1, 0, "Update", "Only provided for enums."
    );
    qmlRegisterSingletonType<SystemUpdate>(
        uri, 1, 0, "SystemUpdate", suSingletonProvider
    );
    qmlRegisterType<Click::Manager>(uri, 1, 0, "ClickManager");
    qRegisterMetaType<UpdateModel*>("UpdateModel*");
    qRegisterMetaType<UpdateModelFilter*>("UpdateModelFilter*");
}
