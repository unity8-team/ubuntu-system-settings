/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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

#include <QDebug>
#include <QtQml>
#include <QtQml/QQmlContext>
#include <QObject>
#include <QTest>
#include <QString>
#include "update.h"

using namespace UpdatePlugin;

//Q_DECLARE_TYPE(Update)

class UpdateTest: public QObject
{
    Q_OBJECT

public:
    UpdateTest() {};

private Q_SLOTS:
    void testCompareVersion();
};

void UpdateTest::testCompareVersion()
{
    Update app;
    app.initializeApplication("package.name", "title", "1.1");
    QCOMPARE(app.updateRequired(), false);
    QString version("1.4");
    app.setRemoteVersion(version);
    QCOMPARE(app.updateRequired(), true);
}

QTEST_MAIN(UpdateTest)
#include "tst_update.moc"
