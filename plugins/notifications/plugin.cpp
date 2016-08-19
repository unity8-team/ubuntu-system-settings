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
*/

#include "plugin.h"

// Qt
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>

#include "general_notification_settings.h"
#include "click_applications_model.h"
#include "click_applications_notify_model.h"

#define MAKE_SINGLETON_FACTORY(type) \
    static QObject* type##_singleton_factory(QQmlEngine* engine, QJSEngine* scriptEngine) { \
        Q_UNUSED(engine); \
        Q_UNUSED(scriptEngine); \
        return new type(); \
    }

MAKE_SINGLETON_FACTORY(GeneralNotificationSettings)
MAKE_SINGLETON_FACTORY(ClickApplicationsModel)

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.Notifications"));

    qmlRegisterSingletonType<GeneralNotificationSettings>(uri, 1, 0, "GeneralNotificationSettings", GeneralNotificationSettings_singleton_factory);
    qmlRegisterSingletonType<ClickApplicationsModel>(uri, 1, 0, "ClickApplicationsModel", ClickApplicationsModel_singleton_factory);
    qmlRegisterType<ClickApplicationsNotifyModel>(uri, 1, 0, "ClickApplicationsNotifyModel");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
