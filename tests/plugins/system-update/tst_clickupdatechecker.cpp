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
#include <QTest>

#include "mockclickservertestcase.h"

#include "clickupdatemetadata.h"
#include "clickupdatechecker.h"

class TstClickUpdateChecker
    : public QObject
    , public MockClickServerTestCase
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        startMockClickServer();
    }
    void cleanupTestCase()
    {
        m_mockclickserver.close();
    }
    void init()
    {
        m_instance = new UpdatePlugin::ClickUpdateChecker();
    }
    void cleanup()
    {
        delete m_instance;
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
    void testCheckEmitsClicks()
    {
        QSignalSpy checkCompletedSpy(m_instance, SIGNAL(checkCompleted()));

        QList<const UpdatePlugin::ClickUpdateMetadata*> metas;
        QObject::connect(
            m_instance, &UpdatePlugin::ClickUpdateChecker::updateAvailable,
            [&](const UpdatePlugin::ClickUpdateMetadata *value) {
                metas.append(value);
            }
        );

        m_instance->check();

        QTRY_VERIFY(metas.size() == 2);
        QCOMPARE(checkCompletedSpy.count(), 1);

        // // Value set by mock click command.
        // QCOMPARE(m->localVersion(), QLatin1String("0.1"));

        // // Values set by mock click server.
        // QCOMPARE(m->name(), QLatin1String("com.ubuntu.developer.testclick"));
        // QCOMPARE(m->remoteVersion(), QLatin1String("2.0"));
        // QCOMPARE(m->downloadUrl(), QLatin1String("http://localhost:9009/download"));
        // QCOMPARE(m->binaryFilesize(), (uint)9000);
        // QCOMPARE(m->downloadSha512(), QLatin1String("1232223sdfdsffs"));
        // QCOMPARE(m->changelog(), QLatin1String("New version!"));
        // QCOMPARE(m->changelog(), QLatin1String("New version!"));
        // QCOMPARE(m->title(), QLatin1String("Test Click App"));

        // // This comparison is also made against values from the mock click
        // // update server, but they are fetched using a different code path.
        // QCOMPARE(m->clickToken(), QLatin1String("Mock-X-Click-Token"));
    }
    void testCheckClick()
    {
        QSignalSpy checkCompletedSpy(m_instance, SIGNAL(checkCompleted()));
        m_instance->check();
        QVERIFY(checkCompletedSpy.wait());

        const UpdatePlugin::ClickUpdateMetadata *m;
        QObject::connect(
            m_instance, &UpdatePlugin::ClickUpdateChecker::updateAvailable,
            [&](const UpdatePlugin::ClickUpdateMetadata *value) {
                m = value;
            }
        );
        m_instance->check("com.ubuntu.developer.testclick");

        QTRY_VERIFY(m);
    }
private:
    UpdatePlugin::ClickUpdateChecker *m_instance;
};

QTEST_MAIN(TstClickUpdateChecker)
#include "tst_clickupdatechecker.moc"

