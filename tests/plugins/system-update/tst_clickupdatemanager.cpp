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

#include <QSignalSpy>
#include <QSqlError>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QTest>

#include "mockclickservertestcase.h"

#include "updatedb.h"
#include "clicktokendownloader.h"
#include "clickupdatemanager.h"

using namespace UpdatePlugin;

class TstClickUpdateManager
    : public QObject
    , public MockClickServerTestCase
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        // The server starts with a default setting that will have it
        // respond with two click updates.
        startMockClickServer();
    }
    void cleanupTestCase()
    {
        stopMockClickServer();
    }
    void init()
    {
        m_dir = new QTemporaryDir();
        QVERIFY(m_dir->isValid());
        m_dbfile = m_dir->path() + "/cupdatemanagerstore.db";

        m_instance = new ClickUpdateManager(m_dbfile);
        m_db = new UpdateDb(m_dbfile);
    }
    void cleanup()
    {
        m_instance->deleteLater();
        m_db->deleteLater();
        delete m_dir;
    }
    void testCheckCompletes()
    {
        m_instance->check();
        QSignalSpy checkCompletedSpy(m_instance, SIGNAL(checkCompleted()));
        QVERIFY(checkCompletedSpy.wait());
    }
    void testNoUpdates()
    {
        m_mockclickserver.close();
        QStringList args;
        args << "-r [{\"path\": \"*\", \"content\": [],"
                "\"status_code\": 200}]";
        startMockClickServer(args);

        QSignalSpy checkCompletedSpy(m_instance, SIGNAL(checkCompleted()));
        m_instance->check();
        QVERIFY(checkCompletedSpy.wait());

        m_mockclickserver.close();
        startMockClickServer();
    }
    void testCancelCheck()
    {
        m_instance->check();
        QSignalSpy checkCanceledSpy(m_instance, SIGNAL(checkCanceled()));
        m_instance->cancel();
        QVERIFY(checkCanceledSpy.wait());
    }
    void testFailedCheck()
    {
        m_mockclickserver.close();

        m_instance->check();
        QSignalSpy checkFailedSpy(m_instance, SIGNAL(checkFailed()));
        QVERIFY(checkFailedSpy.wait());

        startMockClickServer();
    }
    void testStoresClick()
    {
        QSignalSpy checkCompletedSpy(m_instance, SIGNAL(checkCompleted()));

        m_instance->check();

        QTRY_COMPARE(checkCompletedSpy.count(), 1);

        int actualStoreSize = 0;
        QSqlQuery query = m_db->db().exec("SELECT * FROM updates");

        // while (query.next()) {
        //     QVERIFY(query.isValid());
        //     QCOMPARE(query.value(0).toString(), UpdateModel::KIND_CLICK);
        //     actualStoreSize++;
        // }

        // QCOMPARE(actualStoreSize, 2);
        // m_db->db().close();
    }
    void testCheckClick()
    {
        QSignalSpy checkCompletedSpy(m_instance, SIGNAL(checkCompleted()));
        m_instance->check();
        QVERIFY(checkCompletedSpy.wait());

        // const ClickUpdateMetadata *m;
        // QObject::connect(
        //     m_instance, &ClickUpdateManager::updateAvailable,
        //     [&](const ClickUpdateMetadata *value) {
        //         m = value;
        //     }
        // );
        // m_instance->check("com.ubuntu.developer.testclick");

        // QTRY_VERIFY(m);
    }
    void testCheckRequired()
    {
        QSqlQuery q(m_db->db());
        QDateTime longAgo = QDateTime::currentDateTime().addMonths(-1).addDays(-1).toUTC();

        q.prepare("REPLACE INTO meta (checked_at_utc) VALUES (:checked_at_utc)");
        q.bindValue(":checked_at_utc", longAgo.toMSecsSinceEpoch());

        QVERIFY(q.exec());

        QVERIFY(m_instance->isCheckRequired());
    }
    void testNotRequiredAfterCheck()
    {
        QSignalSpy checkCompletedSpy(m_instance, SIGNAL(checkCompleted()));
        m_instance->check();
        QVERIFY(checkCompletedSpy.wait());
        QVERIFY(!m_instance->isCheckRequired());
    }
private:
    ClickUpdateManager *m_instance;
    UpdateDb *m_db;
    QTemporaryDir *m_dir;
    QString m_dbfile;
};

QTEST_MAIN(TstClickUpdateManager)
#include "tst_clickupdatemanager.moc"

