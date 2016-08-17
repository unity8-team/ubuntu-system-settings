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
#include "click.h"
#include "storageabout.h"
#include "systemimage.h"

static QObject *siSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new QSystemImage;
}

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.StorageAbout"));

    qRegisterMetaType<ClickModel::Roles>();
    qmlRegisterType<ClickModel>(uri, 1, 0, "ClickRoles");
    qmlRegisterType<StorageAbout>(uri, 1, 0, "UbuntuStorageAboutPanel");
    qmlRegisterSingletonType<QSystemImage>(
        uri, 1, 0, "SystemImage", siSingletonProvider
    );
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
