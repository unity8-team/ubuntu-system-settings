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

#include <QDate>
#include <QSignalSpy>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QTime>
#include <QTimeZone>
#include <QTest>

#include "clickupdatemetadata.h"
#include "systemupdate.h"
#include "updatestore.h"

using namespace UpdatePlugin;

class TstUpdateStore : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_dir = new QTemporaryDir();
        QVERIFY(m_dir->isValid());
        m_dbfile = m_dir->path() + "/cupdatesstore.db";
        m_instance = new UpdateStore(m_dbfile);
    }
    void cleanup()
    {
        delete m_instance;
        delete m_dir;
    }
    void testLastCheck()
    {
        QCOMPARE(m_instance->lastCheckDate().isValid(), false);

        // 29th of February 2016 at 20:00 UTC+2
        QDateTime d(QDate(2016, 2, 29), QTime(20,0), QTimeZone(2 * 3600));

        // Same date, in UTC.
        QDateTime dUtc(QDate(2016, 2, 29), QTime(18,0), Qt::UTC);

        m_instance->setLastCheckDate(d);

        QCOMPARE(m_instance->lastCheckDate(), dUtc);
    }
    void testUpdateLifecycle()
    {
        ClickUpdateMetadata m;
        QStringList mc;
        mc << "ls" << "la";
        m.setRevision(42);
        m.setName("com.ubuntu.testapp");
        m.setLocalVersion("0.1");
        m.setRemoteVersion("0.2");
        m.setTitle("Test App");
        m.setBinaryFilesize(1000);
        m.setIconUrl("http://example.org/testapp.png");
        m.setDownloadUrl("http://example.org/testapp.click");
        m.setChangelog("* Fixed all bugs * Introduced new bugs");
        m.setClickToken("Mock-X-Click-Token");
        m.setCommand(mc);
        m.setDownloadSha512("987654323456789");

        ClickUpdateMetadata m2;
        m2.setRevision(100);
        m2.setName("com.ubuntu.myapp");
        m2.setLocalVersion("1.1");
        m2.setRemoteVersion("1.2");
        m2.setTitle("My app");
        m2.setBinaryFilesize(2000);
        m2.setIconUrl("http://example.org/myapp.png");
        m2.setDownloadUrl("http://example.org/myapp.click");
        m2.setChangelog("* First version");
        m2.setClickToken("Mock-X-Click-Token");
        m2.setCommand(mc);
        m2.setDownloadSha512("293847");

        // Add a click app
        m_instance->add(&m);

        int size = 0;
        QVERIFY(m_instance->openDb());
        QSqlQuery q = m_instance->db().exec("SELECT * FROM updates");

        while (q.next()) {
            QVERIFY(q.isValid());
            QCOMPARE(q.value(0).toString(), m_instance->KIND_CLICK);
            QCOMPARE(q.value(1).toString(), m.name());
            QCOMPARE(q.value(2).toString(), m.localVersion());
            QCOMPARE(q.value(3).toString(), m.remoteVersion());
            QCOMPARE(q.value(4).toInt(), m.revision());
            QCOMPARE(q.value(5).toString(), QString("pending"));
            QCOMPARE(q.value(8).toString(), m.title());
            QCOMPARE(q.value(9).toString(), m.downloadSha512());
            QCOMPARE(q.value(10).toUInt(), m.binaryFilesize());
            QCOMPARE(q.value(11).toString(), m.iconUrl());
            QCOMPARE(q.value(12).toString(), m.downloadUrl());
            QCOMPARE(q.value(13).toString(), m.command().join(" "));
            QCOMPARE(q.value(14).toString(), m.changelog());
            QCOMPARE(q.value(15).toString(), m.clickToken());
            QCOMPARE(q.value(16).toString(), QString("available"));
            QCOMPARE(q.value(17).toBool(), false);
            size++;
        }

        QCOMPARE(size, 1);
        m_instance->db().close();

        // We had to refresh tokens, so we re-add the clickmetadata.
        m.setClickToken("New-Mock-X-Click-Token");
        m_instance->add(&m);

        size = 0;
        QVERIFY(m_instance->openDb());
        QSqlQuery q2 = m_instance->db().exec("SELECT * FROM updates");

        while (q2.next()) {
            QVERIFY(q2.isValid());
            QCOMPARE(q2.value(0).toString(), m_instance->KIND_CLICK);
            QCOMPARE(q2.value(1).toString(), m.name());
            QCOMPARE(q2.value(2).toString(), m.localVersion());
            QCOMPARE(q2.value(3).toString(), m.remoteVersion());
            QCOMPARE(q2.value(4).toInt(), m.revision());
            QCOMPARE(q2.value(5).toString(), QString("pending"));
            QCOMPARE(q2.value(8).toString(), m.title());
            QCOMPARE(q2.value(9).toString(), m.downloadSha512());
            QCOMPARE(q2.value(10).toUInt(), m.binaryFilesize());
            QCOMPARE(q2.value(11).toString(), m.iconUrl());
            QCOMPARE(q2.value(12).toString(), m.downloadUrl());
            QCOMPARE(q2.value(13).toString(), m.command().join(" "));
            QCOMPARE(q2.value(14).toString(), m.changelog());
            QCOMPARE(q2.value(15).toString(), m.clickToken());
            QCOMPARE(q2.value(16).toString(), QString("available"));
            QCOMPARE(q2.value(17).toBool(), false);
            size++;
        }

        QCOMPARE(size, 1);
        m_instance->db().close();

        // Add second click app
        m_instance->add(&m2);

        size = 0;
        QVERIFY(m_instance->openDb());
        QSqlQuery q3 = m_instance->db().exec("SELECT * FROM updates");

        while (q3.next()) {
            ClickUpdateMetadata *target;

            if (q3.value(1).toString() == m.name())
                target = &m;
            else
                target = &m2;

            QVERIFY(q3.isValid());
            QCOMPARE(q3.value(0).toString(), m_instance->KIND_CLICK);
            QCOMPARE(q3.value(1).toString(), target->name());
            QCOMPARE(q3.value(2).toString(), target->localVersion());
            QCOMPARE(q3.value(3).toString(), target->remoteVersion());
            QCOMPARE(q3.value(4).toInt(), target->revision());
            QCOMPARE(q3.value(5).toString(), QString("pending"));
            QCOMPARE(q3.value(8).toString(), target->title());
            QCOMPARE(q3.value(9).toString(), target->downloadSha512());
            QCOMPARE(q3.value(10).toUInt(), target->binaryFilesize());
            QCOMPARE(q3.value(11).toString(), target->iconUrl());
            QCOMPARE(q3.value(12).toString(), target->downloadUrl());
            QCOMPARE(q3.value(13).toString(), target->command().join(" "));
            QCOMPARE(q3.value(14).toString(), target->changelog());
            QCOMPARE(q3.value(15).toString(), target->clickToken());
            QCOMPARE(q3.value(16).toString(), QString("available"));
            QCOMPARE(q3.value(17).toBool(), false);
            size++;
        }

        QCOMPARE(size, 2);
        m_instance->db().close();

        // Mark as installed
        m_instance->markInstalled(m.name(), m.revision());

        size = 0;
        QVERIFY(m_instance->openDb());
        QSqlQuery q4 = m_instance->db().exec("SELECT * FROM updates WHERE state='installed'");

        while (q4.next()) {
            QVERIFY(q4.isValid());
            QCOMPARE(q4.value(0).toString(), m_instance->KIND_CLICK);
            QCOMPARE(q4.value(1).toString(), m.name());
            QCOMPARE(q4.value(2).toString(), m.localVersion());
            QCOMPARE(q4.value(3).toString(), m.remoteVersion());
            QCOMPARE(q4.value(4).toInt(), m.revision());
            QCOMPARE(q4.value(5).toString(), QString("installed"));
            QCOMPARE(q4.value(8).toString(), m.title());
            QCOMPARE(q4.value(9).toString(), m.downloadSha512());
            QCOMPARE(q4.value(10).toUInt(), m.binaryFilesize());
            QCOMPARE(q4.value(11).toString(), m.iconUrl());
            QCOMPARE(q4.value(12).toString(), m.downloadUrl());
            QCOMPARE(q4.value(13).toString(), m.command().join(" "));
            QCOMPARE(q4.value(14).toString(), m.changelog());
            QCOMPARE(q4.value(15).toString(), m.clickToken());
            QCOMPARE(q4.value(16).toString(), QString("available"));
            QCOMPARE(q4.value(17).toBool(), false);
            size++;
        }

        QCOMPARE(size, 1);
        m_instance->db().close();

        QCOMPARE(size, 1);
        m_instance->db().close();

        size = 0;
        QVERIFY(m_instance->openDb());
        QSqlQuery q5 = m_instance->db().exec("SELECT * FROM updates WHERE state='pending'");

        while (q5.next()) {
            QVERIFY(q5.isValid());
            QCOMPARE(q5.value(0).toString(), m_instance->KIND_CLICK);
            QCOMPARE(q5.value(1).toString(), m2.name());
            QCOMPARE(q5.value(2).toString(), m2.localVersion());
            QCOMPARE(q5.value(3).toString(), m2.remoteVersion());
            QCOMPARE(q5.value(4).toInt(), m2.revision());
            QCOMPARE(q5.value(5).toString(), QString("pending"));
            QCOMPARE(q5.value(8).toString(), m2.title());
            QCOMPARE(q5.value(9).toString(), m2.downloadSha512());
            QCOMPARE(q5.value(10).toUInt(), m2.binaryFilesize());
            QCOMPARE(q5.value(11).toString(), m2.iconUrl());
            QCOMPARE(q5.value(12).toString(), m2.downloadUrl());
            QCOMPARE(q5.value(13).toString(), m2.command().join(" "));
            QCOMPARE(q5.value(14).toString(), m2.changelog());
            QCOMPARE(q5.value(15).toString(), m2.clickToken());
            QCOMPARE(q5.value(16).toString(), QString("available"));
            QCOMPARE(q5.value(17).toBool(), false);
            size++;
        }

        QCOMPARE(size, 1);
        m_instance->db().close();
    }
    void testStoreChanged()
    {
        ClickUpdateMetadata m;

        // set minimum required
        m.setName("test.app");
        m.setRevision(1);

        QSignalSpy storeChangedSpy(m_instance, SIGNAL(changed()));

        m_instance->add(&m);
        QTRY_COMPARE(storeChangedSpy.count(), 1);

        QSignalSpy storeItemChangedSpy(m_instance, SIGNAL(itemChanged(QString, int)));
        m_instance->markInstalled(m.name(), m.revision());
        QTRY_COMPARE(storeItemChangedSpy.count(), 1);
    }
    void testPruning()
    {
        ClickUpdateMetadata recentUpdate;
        recentUpdate.setName("new.app");
        recentUpdate.setRevision(1);

        ClickUpdateMetadata oldUpdate;
        oldUpdate.setName("old.app");
        oldUpdate.setRevision(1);

        m_instance->add(&recentUpdate);
        m_instance->add(&oldUpdate);

        m_instance->markInstalled(recentUpdate.name(), recentUpdate.revision());
        m_instance->markInstalled(oldUpdate.name(), oldUpdate.revision());

        // Change update date directly in the db
        QVERIFY(m_instance->openDb());
        QSqlQuery q(m_instance->db());
        q.prepare("UPDATE updates SET updated_at_utc = :updated WHERE id = :appid");
        QDateTime longAgo = QDateTime::currentDateTime().addMonths(-1).addDays(-1).toUTC();

        q.bindValue(":updated", longAgo.toMSecsSinceEpoch());
        q.bindValue(":appid", oldUpdate.name());
        q.exec();

        m_instance->pruneDb();

        QVERIFY(m_instance->openDb());
        QSqlQuery q1(m_instance->db());
        q1.exec("SELECT * FROM updates");
        int size = 0;
        while (q1.next()) {
            QVERIFY(q1.isValid());
            QCOMPARE(q1.value(1).toString(), recentUpdate.name());
            size++;
        }
        QCOMPARE(size, 1);
    }
    void testSystemUpdate()
    {
        // We basically add system updates using a freehand API,
        // since version information can change from channel to
        // channel. We test that here.
        QString kind("system");
        QString uniqueIdentifier("_ubuntu");
        int revision(1);
        QString version("OTA-42");
        QString changelog("Changes");
        QString title("Ubuntu Touch");
        QString iconUrl("distributor-logo.svg");
        int binarySize(1000);
        m_instance->add(kind, uniqueIdentifier, revision, version, changelog,
                        title, iconUrl, binarySize, false);

        QVERIFY(m_instance->openDb());
        QSqlQuery q(m_instance->db());
        q.exec("SELECT * FROM updates");
        int size = 0;
        while (q.next()) {
            QVERIFY(q.isValid());
            QCOMPARE(q.value(1).toString(), uniqueIdentifier);
            size++;
        }
        QCOMPARE(size, 1);
    }
    void testMarkInstalled()
    {
        ClickUpdateMetadata m;
        m.setName("test.app");
        m.setRevision(1);

        QSignalSpy storeChangedSpy(m_instance, SIGNAL(changed()));
        QSignalSpy storeItemChangedSpy(m_instance, SIGNAL(itemChanged(QString, int)));

        m_instance->add(&m);
        m_instance->markInstalled(m.name(), m.revision());
        QTRY_COMPARE(storeChangedSpy.count(), 1);
        QTRY_COMPARE(storeItemChangedSpy.count(), 1);

        QVERIFY(m_instance->openDb());
        QSqlQuery q(m_instance->db());
        q.exec("SELECT state FROM updates");
        while (q.next()) {
            QVERIFY(q.isValid());
            QCOMPARE(q.value(0).toString(), UpdateStore::STATE_INSTALLED);
        }
    }
    void testSetUpdateState()
    {
        ClickUpdateMetadata m;
        m.setName("test.app");
        m.setRevision(1);

        QSignalSpy storeChangedSpy(m_instance, SIGNAL(changed()));
        QSignalSpy storeItemChangedSpy(m_instance, SIGNAL(itemChanged(QString, int)));

        m_instance->add(&m);
        m_instance->setUpdateState(m.name(), m.revision(),
                                   SystemUpdate::UpdateState::StateAvailable);
        QTRY_COMPARE(storeChangedSpy.count(), 1);
        QTRY_COMPARE(storeItemChangedSpy.count(), 1);

        QVERIFY(m_instance->openDb());
        QSqlQuery q(m_instance->db());
        q.exec("SELECT update_state FROM updates");
        while (q.next()) {
            QVERIFY(q.isValid());
            QCOMPARE(q.value(0).toString(), QString("available"));
        }
    }
    void testSetProgress()
    {
        ClickUpdateMetadata m;
        m.setName("test.app");
        m.setRevision(1);

        QSignalSpy storeChangedSpy(m_instance, SIGNAL(changed()));
        QSignalSpy storeItemChangedSpy(m_instance, SIGNAL(itemChanged(QString, int)));

        m_instance->add(&m);
        m_instance->setProgress(m.name(), m.revision(), 50);
        QTRY_COMPARE(storeChangedSpy.count(), 1);
        QTRY_COMPARE(storeItemChangedSpy.count(), 1);

        QVERIFY(m_instance->openDb());
        QSqlQuery q(m_instance->db());
        q.exec("SELECT progress FROM updates");
        while (q.next()) {
            QVERIFY(q.isValid());
            QCOMPARE(q.value(0).toInt(), 50);
        }
    }
private:
    UpdateStore *m_instance;
    QTemporaryDir *m_dir;
    QString m_dbfile;
};

QTEST_MAIN(TstUpdateStore)

#include "tst_updatestore.moc"
