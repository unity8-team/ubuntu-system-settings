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

#include "updatemodel.h"
#include "systemupdate.h"

#include "plugins/system-update/fakeclickmanager.h"
#include "plugins/system-update/fakeimagemanager.h"

#include <QSignalSpy>
#include <QSqlError>
#include <QSqlQuery>
#include <QTest>

using namespace UpdatePlugin;

Q_DECLARE_METATYPE(SystemUpdate::Status)
Q_DECLARE_METATYPE(SystemUpdate::Check)

class TstSystemUpdate : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_model = new UpdateModel(":memory:");
        m_imageManager = new MockImageManager();
        m_clickManager = new MockClickManager();
        // Network Access Manager will not be needed for our tests, so it's 0.
        m_instance = new SystemUpdate(m_model, nullptr, m_imageManager,
                                      m_clickManager);
        m_model->setParent(m_instance);
        m_imageManager->setParent(m_instance);
        m_clickManager->setParent(m_instance);
    }
    void cleanup()
    {
        // Everything is ultimately parented to m_instance, thus destroyed.
        QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        m_instance->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
    }
    void testCheckRequired_data()
    {
        QTest::addColumn<QDateTime>("checkedAt");
        QTest::addColumn<bool>("checkRequired");

        QDateTime now = QDateTime::currentDateTime().toUTC();
        QTest::newRow("Now") << now << false;

        QDateTime belowSpecLimit = QDateTime::currentDateTime().addSecs(-1740).toUTC();
        QTest::newRow("29 mins ago") << belowSpecLimit << false;

        QDateTime aboveSpecLimit = QDateTime::currentDateTime().addSecs(-1860).toUTC();
        QTest::newRow("31 mins ago") << aboveSpecLimit << true;

        QDateTime longAgo = QDateTime::currentDateTime().addMonths(-1).addDays(-1).toUTC();
        QTest::newRow("Long ago") << longAgo << true;
    }
    void testCheckRequired()
    {
        QFETCH(QDateTime, checkedAt);
        QFETCH(bool, checkRequired);

        QSqlQuery q(m_model->db()->db());

        q.prepare("REPLACE INTO meta (checked_at_utc) VALUES (:checked_at_utc)");
        q.bindValue(":checked_at_utc", checkedAt.toMSecsSinceEpoch());

        QVERIFY(q.exec());

        QCOMPARE(m_instance->isCheckRequired(), checkRequired);
    }
    void testUpdates()
    {
        QCOMPARE(m_instance->updates(), m_model);
    }
    void testPendingUpdates()
    {
        QVERIFY(m_instance->pendingUpdates());
    }
    void testClickUpdates()
    {
        QVERIFY(m_instance->clickUpdates());
    }
    void testImageUpdates()
    {
        QVERIFY(m_instance->imageUpdates());
    }
    void testInstalledUpdates()
    {
        QVERIFY(m_instance->installedUpdates());
    }
    void testStatus_data()
    {
        QTest::addColumn<bool>("networkError");
        QTest::addColumn<bool>("serverError");
        QTest::addColumn<bool>("checkingClicks");
        QTest::addColumn<bool>("checkingImage");
        QTest::addColumn<SystemUpdate::Status>("targetStatus");

        QTest::newRow("Idle") << false << false << false << false << SystemUpdate::Status::StatusIdle;
        QTest::newRow("Checking clicks") << false << false << true << false << SystemUpdate::Status::StatusCheckingClickUpdates;
        QTest::newRow("Checking image") << false << false << false << true << SystemUpdate::Status::StatusCheckingSystemUpdates;
        QTest::newRow("Checking all") << false << false << true << true << SystemUpdate::Status::StatusCheckingAllUpdates;
        QTest::newRow("Checking all (network fail)") << true << false << true << true << SystemUpdate::Status::StatusNetworkError;
        QTest::newRow("Checking all (server fail)") << false << true << true << true << SystemUpdate::Status::StatusServerError;
        // Network failure is the most important one in a multiple failure scenario.
        QTest::newRow("Checking all (multiple fail)") << true << true << true << true << SystemUpdate::Status::StatusNetworkError;
    }
    void testStatus()
    {
        QFETCH(bool, networkError);
        QFETCH(bool, serverError);
        QFETCH(bool, checkingClicks);
        QFETCH(bool, checkingImage);
        QFETCH(SystemUpdate::Status, targetStatus);

        m_clickManager->mockChecking(checkingClicks);
        m_imageManager->mockChecking(checkingImage);
        if (networkError) {
            m_clickManager->mockNetworkError();
        }
        if (serverError) {
            m_clickManager->mockServerError();
        }
        QCOMPARE(m_instance->status(), targetStatus);
    }
    void testChecks_data()
    {
        QTest::addColumn<SystemUpdate::Check>("mode");
        QTest::addColumn<bool>("checkingClick");
        QTest::addColumn<bool>("checkingImage");

        QTest::newRow("CheckAutomatic") << SystemUpdate::Check::CheckAutomatic << true << true;
        QTest::newRow("CheckAll") << SystemUpdate::Check::CheckAll << true << true;
        QTest::newRow("CheckClick") << SystemUpdate::Check::CheckClick << true << false;
        QTest::newRow("CheckImage") << SystemUpdate::Check::CheckImage << false << true;
    }
    void testChecks()
    {
        QFETCH(SystemUpdate::Check, mode);
        QFETCH(bool, checkingClick);
        QFETCH(bool, checkingImage);

        m_instance->check(mode);

        QCOMPARE(m_clickManager->checkingForUpdates(), checkingClick);
        QCOMPARE(m_imageManager->checkingForUpdates(), checkingImage);
    }
    void testIntegration()
    {
        /* Do this so as to test all the actual constructors. Since we cannot
        say anything about networking/system image dbus, et. al., we do not
        perform any assertions here, except verifying that the ctor constructed
        a SystemUpdate, and that the dtor destroyed it. */
        SystemUpdate *su = new SystemUpdate();
        QVERIFY(su);
        QTest::qWait(2000); // Wait for things to quiet down.
        QSignalSpy destroyedSpy(su, SIGNAL(destroyed(QObject*)));
        su->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
    }
private:
    SystemUpdate *m_instance = nullptr;
    UpdateModel *m_model = nullptr;
    UpdateModelFilter *m_pending = nullptr;
    UpdateModelFilter *m_clicks = nullptr;
    UpdateModelFilter *m_images = nullptr;
    UpdateModelFilter *m_installed = nullptr;
    MockClickManager *m_clickManager = nullptr;
    MockImageManager *m_imageManager = nullptr;
};

QTEST_MAIN(TstSystemUpdate)
#include "tst_systemupdate.moc"