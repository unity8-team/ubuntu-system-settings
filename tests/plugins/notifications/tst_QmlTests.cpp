/*
 * Copyright 2016 Canonical Ltd.
 *
 * This file is part of system-settings.
 *
 * webbrowser-app is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * webbrowser-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Qt
#include <QtCore/QObject>
#include <QtQml/QtQml>
#include <QtQuickTest/QtQuickTest>

// local
#include "click_applications_model.h"
#include "click_applications_notify_model.h"

class ClickApplicationsModelMock : public ClickApplicationsModel {
    Q_OBJECT

public:
    Q_INVOKABLE void foo() { qWarning() << "[DEBUG] MOCK BAR"; }

protected Q_SLOTS:
    void populateModel() { /* DO NOTHING */ }
};

#define MAKE_SINGLETON_FACTORY(type) \
    static QObject* type##_singleton_factory(QQmlEngine* engine, QJSEngine* scriptEngine) { \
        Q_UNUSED(engine); \
        Q_UNUSED(scriptEngine); \
        return new type(); \
    }

MAKE_SINGLETON_FACTORY(ClickApplicationsModelMock)

int main(int argc, char** argv)
{
    const char* uri = "Ubuntu.SystemSettings.Notifications";
    qmlRegisterSingletonType<ClickApplicationsModel>(uri, 1, 0, "ClickApplicationsModel", ClickApplicationsModelMock_singleton_factory);
    qmlRegisterType<ClickApplicationsNotifyModel>(uri, 1, 0, "ClickApplicationsNotifyModel");

    return quick_test_main(argc, argv, "QmlTests", nullptr);
}

#include "tst_QmlTests.moc"
