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

#include <QProcessEnvironment>
#include <QTest>

#include "helpers.h"

class TstUpdatePluginHelpers : public QObject
{
    Q_OBJECT
private slots:
    void testGetFrameworksDir()
    {
        QCOMPARE(UpdatePlugin::Helpers::getFrameworksDir(),
                env.value("FRAMEWORKS_FOLDER"));
    }
    void testGetAvailableFrameworks()
    {
        QString target("ubuntu-sdk-15.04");
        for(auto const& value: UpdatePlugin::Helpers::getAvailableFrameworks())
             QCOMPARE(QString::fromStdString(value), target);
    }
    void testGetArchitecture()
    {
        QString res = QString::fromStdString(
            UpdatePlugin::Helpers::getArchitecture());
        QVERIFY(!res.isEmpty());
    }
    void testClickMetadataUrl()
    {
        QCOMPARE(UpdatePlugin::Helpers::clickMetadataUrl(),
                 env.value("URL_APPS"));
    }
    void testIsIgnoringCredentials()
    {
        bool shouldIgnore = env.value("IGNORE_CREDENTIALS", "") != "";
        QCOMPARE(UpdatePlugin::Helpers::isIgnoringCredentials(), shouldIgnore);

    }
    void testWhichClick()
    {
        QCOMPARE(UpdatePlugin::Helpers::whichClick(), QLatin1String("click"));
    }
    void testWhichPkcon()
    {
        QCOMPARE(UpdatePlugin::Helpers::whichPkcon(), QLatin1String("pkcon"));
    }

private:
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
};

QTEST_GUILESS_MAIN(TstUpdatePluginHelpers)
#include "tst_helpers.moc"

