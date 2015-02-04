/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Iain Lane <iain.lane@canonical.com>
 */

#include "plugin.h"
#include "securityprivacy.h"
#include "trust-store-model.h"
#include "connectivity.h"

#include <QtQml/QtQml>

static QObject *connectivitySingeltonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    Connectivity *connectivity = new Connectivity();
    return connectivity;
}

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.SecurityPrivacy"));
    qmlRegisterSingletonType<Connectivity>(uri, 1, 0, "Connectivity", connectivitySingeltonProvider);
    qmlRegisterType<SecurityPrivacy>(uri, 1, 0, "UbuntuSecurityPrivacyPanel");
    qmlRegisterType<TrustStoreModel>(uri, 1, 0, "TrustStoreModel");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
