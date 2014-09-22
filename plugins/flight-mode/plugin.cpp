/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Antti Kaijanmäki <antti.kaijanmaki@canonical.com>
 *
 */

#include "plugin.h"

#include <QtQml>
#include <QtQml/QQmlContext>

#include "flight-mode-helper.h"

void BackendPlugin::registerTypes(const char *uri)
{
    // @uri Ubuntu.SystemSettings.FlightMode
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.FlightMode"));
    qmlRegisterType<FlightModeHelper>(uri, 1, 0, "Helper");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
