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

#include "clickupdatemetadata.h"
#include "clickupdatemanager.h"

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
        m_mockclickserver.close();
    }
    void init()
    {
        m_dir = new QTemporaryDir();
        QVERIFY(m_dir->isValid());
        m_dbfile = m_dir->path() + "/cupdatemanagerstore.db";

        m_instance = new UpdatePlugin::ClickUpdateManager(m_dbfile);
        m_store = new UpdatePlugin::UpdateStore(m_dbfile);
    }
    void cleanup()
    {
        m_store->db().close();
        delete m_instance;
        delete m_store;
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

        QVERIFY(!m_instance->errorString().isEmpty());
        qDebug() << "Click update checker said:" << m_instance->errorString();

        startMockClickServer();
    }
    void testStoresClick()
    {
        QSignalSpy checkCompletedSpy(m_instance, SIGNAL(checkCompleted()));

        m_instance->check();

        QTRY_COMPARE(checkCompletedSpy.count(), 1);

        int actualStoreSize = 0;
        QVERIFY(m_store->openDb());
        QSqlQuery query = m_store->db().exec("SELECT * FROM updates");

        while (query.next()) {
            QVERIFY(query.isValid());
            QCOMPARE(query.value(0).toString(), m_store->KIND_CLICK);
            actualStoreSize++;
        }

        QCOMPARE(actualStoreSize, 2);
        m_store->db().close();
    }
    void testCheckClick()
    {
        QSignalSpy checkCompletedSpy(m_instance, SIGNAL(checkCompleted()));
        m_instance->check();
        QVERIFY(checkCompletedSpy.wait());

        // const UpdatePlugin::ClickUpdateMetadata *m;
        // QObject::connect(
        //     m_instance, &UpdatePlugin::ClickUpdateManager::updateAvailable,
        //     [&](const UpdatePlugin::ClickUpdateMetadata *value) {
        //         m = value;
        //     }
        // );
        // m_instance->check("com.ubuntu.developer.testclick");

        // QTRY_VERIFY(m);
    }
private:
    UpdatePlugin::ClickUpdateManager *m_instance;
    UpdatePlugin::UpdateStore *m_store;
    QTemporaryDir *m_dir;
    QString m_dbfile;
};

QTEST_MAIN(TstClickUpdateManager)
#include "tst_clickupdatemanager.moc"

