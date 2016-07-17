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

#include "MockClickManager.h"
#include "MockUpdate.h"
#include "MockUpdateModel.h"
#include "MockSystemImage.h"
#include "MockSystemUpdate.h"

#include <QtQml>

static QObject *siSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new MockSystemImage;
}

static QObject *suSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return MockSystemUpdate::instance();
}

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.Update"));
    qmlRegisterSingletonType<MockSystemImage>(uri, 1, 0, "SystemImage", siSingletonProvider);
    qmlRegisterSingletonType<MockSystemUpdate>(uri, 1, 0, "SystemUpdate", suSingletonProvider);

    qmlRegisterUncreatableType<MockUpdate>(uri, 1, 0, "Update", "Used for enums only.");
    qmlRegisterType<MockClickManager>(uri, 1, 0, "ClickManager");
    qmlRegisterType<MockUpdateModel>(uri, 1, 0, "UpdateModel");
    qmlRegisterType<MockUpdateModelFilter>(uri, 1, 0, "UpdateModelFilter");
}