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
#include "MockCertHandler.h"
#include "MockDbusHelper.h"

#include <QtQml>

#define MAKE_SINGLETON_FACTORY(type) \
    static QObject* type##_singleton_factory(QQmlEngine* engine, QJSEngine* scriptEngine) { \
        Q_UNUSED(engine); \
        Q_UNUSED(scriptEngine); \
        return new type(); \
    }

MAKE_SINGLETON_FACTORY(MockDbusHelper)

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.Wifi"));
    qmlRegisterSingletonType<MockDbusHelper>(uri, 1, 0, "DbusHelper", MockDbusHelper_singleton_factory);
    qmlRegisterType<MockCertificateListModel>(uri, 1, 0, "CertificateListModel");
    qmlRegisterType<MockPrivatekeyListModel>(uri, 1, 0, "PrivatekeyListModel");
    qmlRegisterType<MockPacFileListModel>(uri, 1, 0, "PacFileListModel");
    qmlRegisterType<MockFileHandler>(uri, 1, 0, "FileHandler");
}
