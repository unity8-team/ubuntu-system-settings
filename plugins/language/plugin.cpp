/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
 *          William Hua <william.hua@canonical.com>
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

#include <QtQml>
#include "subset-model.h"
#include "language-plugin.h"

void BackendPlugin::registerTypes(const char *uri)
{
    // @uri Ubuntu.SystemSettings.LanguagePlugin
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.LanguagePlugin"));
    qmlRegisterType<SubsetModel>(uri, 1, 0, "SubsetModel");
    qmlRegisterType<LanguagePlugin>(uri, 1, 0, "UbuntuLanguagePlugin");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
