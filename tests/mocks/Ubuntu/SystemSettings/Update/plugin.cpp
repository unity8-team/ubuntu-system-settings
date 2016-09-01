/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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

#include "plugin.h"

#include "update.h"
#include "utils.h"

#include "MockUpdateManager.h"
#include "MockUpdateModel.h"
#include "MockSystemImage.h"

#include <QtQml>

static QObject *siSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new MockSystemImage;
}

static QObject *umSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new MockUpdateManager;
}

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.Update"));
    qmlRegisterSingletonType<MockUpdateManager>(uri, 1, 0, "UpdateManager", umSingletonProvider);
    qmlRegisterUncreatableType<Update>(uri, 1, 0, "Update", "Used for enums only.");
    qmlRegisterSingletonType<MockSystemImage>(uri, 1, 0, "SystemImage", siSingletonProvider);
    qRegisterMetaType<MockUpdateModel*>("UpdateModel*");
    qRegisterMetaType<MockUpdateModelFilter*>("UpdateModelFilter*");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
    QQmlContext* context = engine->rootContext();
    auto utils = new SystemSettings::Utilities;
    context->setContextProperty("Utilities", utils);
    context->setContextProperty("showAllUI", true);
    context->setContextProperty("pluginOptions", QVariantMap());
}
