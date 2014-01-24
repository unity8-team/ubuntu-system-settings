/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QTest>
#include <QSignalSpy>

#include "update_manager.h"
#include "update.h"

using namespace UpdatePlugin;

class UpdateManagerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testCheckUpdatesNotFound();
    void testRegisterSystemUpdateRequired();
    void testRegisterSystemUpdateNotRequired();
    void testStartDownload();
    void testPauseDownload();

private:
    Update* getUpdate();
};

Update* UpdateManagerTest::getUpdate()
{
    Update *update = new Update(this);
    QString packageName("UbuntuImage");
    update->initializeApplication(packageName, "Ubuntu", QString::number(1));
    update->setSystemUpdate(true);
    QString version(2);
    update->setRemoteVersion(version);
    update->setBinaryFilesize(12345);
    update->setUpdateState(false);
    update->setUpdateAvailable(true);

    return update;
}

void UpdateManagerTest::testCheckUpdatesNotFound()
{
    UpdateManager manager;
    QSignalSpy spy(&manager, SIGNAL(updatesNotFound()));
    QTRY_COMPARE(spy.count(), 0);
    manager.checkUpdates();
    QTRY_COMPARE(spy.count(), 1);
    QTRY_COMPARE(manager.get_apps().size(), 0);
}

void UpdateManagerTest::testRegisterSystemUpdateRequired()
{
    UpdateManager manager;
    QSignalSpy spy(&manager, SIGNAL(modelChanged()));
    QSignalSpy spy2(&manager, SIGNAL(updateAvailableFound()));
    QTRY_COMPARE(spy.count(), 0);
    QTRY_COMPARE(spy2.count(), 0);
    QTRY_COMPARE(manager.get_apps().size(), 0);

    Update *update = getUpdate();

    manager.registerSystemUpdate(update->getPackageName(), update);
    QTRY_COMPARE(spy.count(), 1);
    QTRY_COMPARE(spy2.count(), 1);
    QTRY_COMPARE(manager.get_apps().size(), 1);
    QTRY_COMPARE(manager.get_model().size(), 1);
    Update* app = manager.get_model()[0].value<Update*>();
    QCOMPARE(app->getTitle(), QString("Ubuntu"));
    QCOMPARE(app->updateRequired(), true);
    QCOMPARE(app->getPackageName(), QString("UbuntuImage"));

    update->deleteLater();
}

void UpdateManagerTest::testRegisterSystemUpdateNotRequired()
{
    UpdateManager manager;
    QSignalSpy spy(&manager, SIGNAL(modelChanged()));
    QSignalSpy spy2(&manager, SIGNAL(updateAvailableFound()));
    QSignalSpy spy3(&manager, SIGNAL(updatesNotFound()));
    QTRY_COMPARE(spy.count(), 0);
    QTRY_COMPARE(spy2.count(), 0);
    QTRY_COMPARE(spy3.count(), 0);
    QTRY_COMPARE(manager.get_apps().size(), 0);

    Update *update = getUpdate();
    update->setUpdateAvailable(false);

    manager.registerSystemUpdate(update->getPackageName(), update);
    QTRY_COMPARE(spy.count(), 0);
    QTRY_COMPARE(spy2.count(), 0);
    QTRY_COMPARE(spy3.count(), 1);
    QTRY_COMPARE(manager.get_apps().size(), 0);
    QTRY_COMPARE(manager.get_model().size(), 0);

    update->deleteLater();
}

void UpdateManagerTest::testStartDownload()
{
    UpdateManager manager;
    Update *update = getUpdate();
    manager.registerSystemUpdate(update->getPackageName(), update);
    manager.startDownload(update->getPackageName());
    QTRY_COMPARE(update->updateState(), true);
}

void UpdateManagerTest::testPauseDownload()
{
    UpdateManager manager;
    Update *update = getUpdate();
    manager.registerSystemUpdate(update->getPackageName(), update);
    update->setUpdateState(true);
    manager.pauseDownload(update->getPackageName());
    QTRY_COMPARE(update->updateState(), false);
}


QTEST_MAIN(UpdateManagerTest);
#include "tst_updatemanager.moc"
