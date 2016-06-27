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
#include <QFile>
#include <QSignalSpy>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QTime>
#include <QTimeZone>
#include <QTest>

#include "update.h"
#include "updatedb.h"
#include "updatemodel.h"

using namespace UpdatePlugin;

Q_DECLARE_METATYPE(QSharedPointer<Update>)
Q_DECLARE_METATYPE(UpdateDb::Filter)

class TstUpdateDb : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_dir = new QTemporaryDir();
        QVERIFY(m_dir->isValid());
        m_dbfile = m_dir->path() + "/test_updatedb.db";
        m_instance = new UpdateDb(m_dbfile);
    }
    void cleanup()
    {
        m_instance->deleteLater();
        delete m_dir;
    }
    void cleanupTestCase()
    {
        /* Allow for deleteLater of QObjects in this test to be deleted.
        Workaround for QTBUG-12575. */
        QTest::qWait(200);
    }
    QSharedPointer<Update> createUpdate()
    {
        return QSharedPointer<Update>(new Update);
    }
    void testNoUpdates()
    {
        auto list = m_instance->updates(UpdateDb::Filter::All);
        QCOMPARE(list.size(), 0);
    }
    void testAddUpdate()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);

        QSignalSpy changedSpy(m_instance, SIGNAL(changed()));
        m_instance->add(m);
        QTRY_COMPARE(changedSpy.count(), 1);
    }
    void testRemoveUpdate()
    {

        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);

        QSignalSpy changedSpy(m_instance, SIGNAL(changed()));
        m_instance->add(m);
        m_instance->remove(m);
        QTRY_COMPARE(changedSpy.count(), 2);

        auto list = m_instance->updates(UpdateDb::Filter::All);
        QCOMPARE(list.size(), 0);

    }
    void testFilters_data()
    {
        QTest::addColumn<QSharedPointer<Update> >("a");
        QTest::addColumn<QSharedPointer<Update> >("b");
        QTest::addColumn<QSharedPointer<Update> >("c");
        QTest::addColumn<QSharedPointer<Update> >("d");
        QTest::addColumn<UpdateDb::Filter>("filter");
        QTest::addColumn<QStringList>("ids");

        QSharedPointer<Update> a = createUpdate();
        a->setIdentifier("a");
        a->setKind(Update::Kind::KindClick);

        QSharedPointer<Update> b = createUpdate();
        b->setIdentifier("b");
        b->setKind(Update::Kind::KindImage);

        QSharedPointer<Update> c = createUpdate();
        c->setIdentifier("c");
        c->setInstalled(true);
        c->setKind(Update::Kind::KindClick);

        QSharedPointer<Update> d = createUpdate();
        d->setIdentifier("d");
        d->setInstalled(true);
        d->setKind(Update::Kind::KindImage);

        QStringList all; all << "a" << "b" << "c" << "d";
        QTest::newRow("All") << a << b << c << d << UpdateDb::Filter::All << all;

        QStringList pending; pending << "a" << "b";
        QTest::newRow("Pending") << a << b << c << d << UpdateDb::Filter::Pending << pending;

        QStringList pendingClicks; pendingClicks << "a";
        QTest::newRow("PendingClicks") << a << b << c << d << UpdateDb::Filter::PendingClicks << pendingClicks;

        QStringList pendingImage; pendingImage << "b";
        QTest::newRow("PendingImage") << a << b << c << d << UpdateDb::Filter::PendingImage << pendingImage;

        QStringList installedClicks; installedClicks << "c";
        QTest::newRow("InstalledClicks") << a << b << c << d << UpdateDb::Filter::InstalledClicks << installedClicks;

        QStringList installedImage; installedImage << "d";
        QTest::newRow("InstalledImage") << a << b << c << d << UpdateDb::Filter::InstalledImage << installedImage;

        QStringList installed; installed << "c" << "d";
        QTest::newRow("Installed") << a << b << c << d << UpdateDb::Filter::Installed << installed;
    }
    void testFilters()
    {
        QFETCH(QSharedPointer<Update>, a);
        QFETCH(QSharedPointer<Update>, b);
        QFETCH(QSharedPointer<Update>, c);
        QFETCH(QSharedPointer<Update>, d);
        QFETCH(UpdateDb::Filter, filter);
        QFETCH(QStringList, ids);

        m_instance->add(a);
        m_instance->add(b);
        m_instance->add(c);
        m_instance->add(d);

        QList<QSharedPointer<Update> > updates = m_instance->updates(filter);

        QStringList actualIds;
        for (int i = 0; i < updates.size(); i++) {
            actualIds << updates.at(i)->identifier();
        }

        QCOMPARE(actualIds, ids);
    }
    // void testUpdate()
    // {
    //     Update m;
    //     m.setName("test.app");
    //     m.setRevision(1);

    //     m_instance->add(&m);
    //     m_instance->refresh();

    //     QCOMPARE(m_instance->count(), 1);
    // }
    // void testFilters()
    // {
    //     Update pendingApp;
    //     pendingApp.setName("pending.app");
    //     pendingApp.setRevision(1);

    //     Update installedApp;
    //     installedApp.setName("installed.app");
    //     installedApp.setRevision(1);

    //     m_instance->add(&pendingApp);
    //     m_instance->add(&installedApp);
    //     m_instance->setDownloadId("installed.app", 1, "1234");
    //     m_instance->setInstalled("1234");

    //     m_instance->setFilter(UpdateModel::UpdateTypes::PendingClicksUpdates);
    //     QCOMPARE(m_instance->count(), 1);
    //     QCOMPARE(
    //         m_instance->data(
    //             m_instance->index(0), UpdateModel::IdRole
    //         ).toString(), pendingApp.name()
    //     );

    //     m_instance->setFilter(UpdateModel::UpdateTypes::InstalledClicksUpdates);
    //     QCOMPARE(m_instance->count(), 1);

    //     QCOMPARE(
    //         m_instance->data(
    //             m_instance->index(0), UpdateModel::IdRole
    //         ).toString(),
    //         installedApp.name()
    //     );

    //     m_instance->setFilter(UpdateModel::UpdateTypes::All);
    //     QCOMPARE(m_instance->count(), 2);
    // }
    void testSupersededUpdate()
    {
        QSharedPointer<Update> superseded = createUpdate();
        superseded->setIdentifier("some.app");
        superseded->setRevision(1);
        superseded->setKind(Update::Kind::KindClick);

        QSharedPointer<Update> replacement = createUpdate();
        replacement->setIdentifier("some.app");
        replacement->setRevision(2);
        replacement->setKind(Update::Kind::KindClick);

        m_instance->add(superseded);
        m_instance->add(replacement);

        // We only want the replacement in our db of pending updates.
        QList<QSharedPointer<Update> > list = m_instance->updates(UpdateDb::Filter::PendingClicks);
        QCOMPARE(list.count(), 1);
    }
    // void testRoles()
    // {
    //     using namespace UpdatePlugin;
    //     Update app;
    //     QStringList mc;
    //     mc << "ls" << "la";

    //     app.setName("com.ubuntu.testapp");
    //     app.setLocalVersion("0.1");
    //     app.setRemoteVersion("0.2");
    //     app.setRevision(42);
    //     app.setTitle("Test App");
    //     app.setDownloadSha512("987654323456789");
    //     app.setBinaryFilesize(1000);
    //     app.setIconUrl("http://example.org/testapp.png");
    //     app.setDownloadUrl("http://example.org/testapp.click");
    //     app.setCommand(mc);
    //     app.setChangelog("* Fixed all bugs * Introduced new bugs");
    //     app.setToken("token");

    //     QSignalSpy storeChangedSpy(m_instance, SIGNAL(changed()));
    //     m_instance->add(&app);
    //     m_instance->setDownloadId(app.name(), app.revision(), "1234");

    //     QTRY_COMPARE(storeChangedSpy.count(), 1);
    //     m_instance->setProgress("1234", 50);

    //     m_instance->refresh();

    //     QModelIndex idx = m_instance->index(0);

    //     QCOMPARE(m_instance->data(idx, UpdateModel::KindRole).toString(), UpdateModel::KIND_CLICK);
    //     QCOMPARE(m_instance->data(idx, UpdateModel::IdRole).toString(), app.name());
    //     QCOMPARE(m_instance->data(idx, UpdateModel::LocalVersionRole).toString(), app.localVersion());
    //     QCOMPARE(m_instance->data(idx, UpdateModel::RemoteVersionRole).toString(), app.remoteVersion());
    //     QCOMPARE(m_instance->data(idx, UpdateModel::RevisionRole).toInt(), app.revision());
    //     QCOMPARE(m_instance->data(idx, UpdateModel::TitleRole).toString(), app.title());
    //     QCOMPARE(m_instance->data(idx, UpdateModel::DownloadHashRole).toString(), app.downloadSha512());
    //     QCOMPARE(m_instance->data(idx, UpdateModel::SizeRole).toUInt(), app.binaryFilesize());
    //     QCOMPARE(m_instance->data(idx, UpdateModel::IconUrlRole).toString(), app.iconUrl());
    //     QCOMPARE(m_instance->data(idx, UpdateModel::DownloadUrlRole).toString(), app.downloadUrl());
    //     QCOMPARE(m_instance->data(idx, UpdateModel::CommandRole).toStringList(), app.command());
    //     QCOMPARE(m_instance->data(idx, UpdateModel::ChangelogRole).toString(), app.changelog());
    //     QCOMPARE(m_instance->data(idx, UpdateModel::InstalledRole).toBool(), false);
    //     QCOMPARE(m_instance->data(idx, UpdateModel::AutomaticRole).toBool(), app.automatic());
    //     QCOMPARE(m_instance->data(idx, UpdateModel::DownloadIdRole).toString(), QString("1234"));
    //     QCOMPARE(m_instance->data(idx, UpdateModel::ErrorRole).toString(), QString(""));

    //     int stateInt = m_instance->data(idx, UpdateModel::UpdateStateRole).toInt();
    //     SystemUpdate::UpdateState u = (SystemUpdate::UpdateState) stateInt;
    //     QCOMPARE(u, SystemUpdate::UpdateState::StateAvailable);

    //     QCOMPARE(m_instance->data(idx, UpdateModel::ProgressRole).toInt(), 50);

    //     // Verify that the date ain't empty.
    //     QVERIFY(!m_instance->data(idx, UpdateModel::CreatedAtRole).toString().isEmpty());
    // }
    // void testFilter_data()
    // {
    //     QTest::addColumn<int>("filter");
    //     QTest::newRow("Pending") << (int) UpdateModel::UpdateTypes::Pending;
    //     QTest::newRow("PendingClicksUpdates") << (int) UpdateModel::UpdateTypes::PendingClicksUpdates;
    //     QTest::newRow("PendingSystemUpdates") << (int) UpdateModel::UpdateTypes::PendingSystemUpdates;
    //     QTest::newRow("InstalledClicksUpdates") << (int) UpdateModel::UpdateTypes::InstalledClicksUpdates;
    //     QTest::newRow("InstalledSystemUpdates") << (int) UpdateModel::UpdateTypes::InstalledSystemUpdates;
    //     QTest::newRow("Installed") << (int) UpdateModel::UpdateTypes::Installed;
    // }
    // void testFilter()
    // {
    //     QFETCH(int, filter);

    //     QSignalSpy filterChangedSpy(m_instance, SIGNAL(filterChanged()));
    //     m_instance->setFilter((UpdateModel::UpdateTypes) filter);
    //     QTRY_COMPARE(filterChangedSpy.count(), 1);
    //     QCOMPARE((int) m_instance->filter(), filter);
    // }
    void testLastCheck_data() {
        QTest::addColumn<QDateTime>("set");
        QTest::addColumn<QDateTime>("target");

        QDateTime otherTz(QDate(2016, 2, 29), QTime(20, 0), QTimeZone(2 * 3600));
        QDateTime utcTz(QDate(2016, 2, 29), QTime(18, 0), Qt::UTC);
        QTest::newRow("Different TZ") << otherTz << utcTz;

        QTest::newRow("UTC TZ") << QDateTime(QDate(2016, 2, 29), QTime(18, 0), Qt::UTC)
                                << QDateTime(QDate(2016, 2, 29), QTime(18, 0), Qt::UTC);
    }
    void testLastCheck()
    {
        QFETCH(QDateTime, set);
        QFETCH(QDateTime, target);
        QCOMPARE(m_instance->lastCheckDate().isValid(), false);

        m_instance->setLastCheckDate(set);

        QCOMPARE(m_instance->lastCheckDate(), target);
    }
    // void testUpdateLifecycle()
    // {
    //     Update m;
    //     QStringList mc;
    //     mc << "ls" << "la";
    //     m.setRevision(42);
    //     m.setName("com.ubuntu.testapp");
    //     m.setLocalVersion("0.1");
    //     m.setRemoteVersion("0.2");
    //     m.setTitle("Test App");
    //     m.setBinaryFilesize(1000);
    //     m.setIconUrl("http://example.org/testapp.png");
    //     m.setDownloadUrl("http://example.org/testapp.click");
    //     m.setChangelog("* Fixed all bugs * Introduced new bugs");
    //     m.setToken("token");
    //     m.setCommand(mc);
    //     m.setDownloadSha512("987654323456789");

    //     Update m2;
    //     m2.setRevision(100);
    //     m2.setName("com.ubuntu.myapp");
    //     m2.setLocalVersion("1.1");
    //     m2.setRemoteVersion("1.2");
    //     m2.setTitle("My app");
    //     m2.setBinaryFilesize(2000);
    //     m2.setIconUrl("http://example.org/myapp.png");
    //     m2.setDownloadUrl("http://example.org/myapp.click");
    //     m2.setChangelog("* First version");
    //     m2.setToken("token");
    //     m2.setCommand(mc);
    //     m2.setDownloadSha512("293847");

    //     // Add an app
    //     m_instance->add(&m);

    //     int size = 0;
    //     QSqlQuery q = m_instance->db().exec("SELECT * FROM updates");

    //     while (q.next()) {
    //         QVERIFY(q.isValid());
    //         QCOMPARE(q.value(0).toString(), m_instance->KIND_CLICK);
    //         QCOMPARE(q.value(1).toString(), m.name());
    //         QCOMPARE(q.value(2).toString(), m.localVersion());
    //         QCOMPARE(q.value(3).toString(), m.remoteVersion());
    //         QCOMPARE(q.value(4).toInt(), m.revision());
    //         QCOMPARE(q.value(5).toString(), QString("pending"));
    //         QCOMPARE(q.value(8).toString(), m.title());
    //         QCOMPARE(q.value(9).toString(), m.downloadSha512());
    //         QCOMPARE(q.value(10).toUInt(), m.binaryFilesize());
    //         QCOMPARE(q.value(11).toString(), m.iconUrl());
    //         QCOMPARE(q.value(12).toString(), m.downloadUrl());
    //         QCOMPARE(q.value(13).toString(), m.command().join(" "));
    //         QCOMPARE(q.value(14).toString(), m.changelog());
    //         QCOMPARE(q.value(15).toString(), m.token());
    //         QCOMPARE(q.value(16).toString(), QString("available"));
    //         QCOMPARE(q.value(17).toBool(), false);
    //         size++;
    //     }

    //     QCOMPARE(size, 1);

    //     // We had to refresh tokens, so we re-add the update.
    //     m.setToken("newtoken");
    //     m_instance->add(&m);

    //     size = 0;
    //     QSqlQuery q2 = m_instance->db().exec("SELECT * FROM updates");

    //     while (q2.next()) {
    //         QVERIFY(q2.isValid());
    //         QCOMPARE(q2.value(0).toString(), m_instance->KIND_CLICK);
    //         QCOMPARE(q2.value(1).toString(), m.name());
    //         QCOMPARE(q2.value(2).toString(), m.localVersion());
    //         QCOMPARE(q2.value(3).toString(), m.remoteVersion());
    //         QCOMPARE(q2.value(4).toInt(), m.revision());
    //         QCOMPARE(q2.value(5).toString(), QString("pending"));
    //         QCOMPARE(q2.value(8).toString(), m.title());
    //         QCOMPARE(q2.value(9).toString(), m.downloadSha512());
    //         QCOMPARE(q2.value(10).toUInt(), m.binaryFilesize());
    //         QCOMPARE(q2.value(11).toString(), m.iconUrl());
    //         QCOMPARE(q2.value(12).toString(), m.downloadUrl());
    //         QCOMPARE(q2.value(13).toString(), m.command().join(" "));
    //         QCOMPARE(q2.value(14).toString(), m.changelog());
    //         QCOMPARE(q2.value(15).toString(), m.token());
    //         QCOMPARE(q2.value(16).toString(), QString("available"));
    //         QCOMPARE(q2.value(17).toBool(), false);
    //         size++;
    //     }

    //     QCOMPARE(size, 1);

    //     // Add second app
    //     m_instance->add(&m2);

    //     size = 0;
    //     QSqlQuery q3 = m_instance->db().exec("SELECT * FROM updates");

    //     while (q3.next()) {
    //         Update *target;

    //         if (q3.value(1).toString() == m.name())
    //             target = &m;
    //         else
    //             target = &m2;

    //         QVERIFY(q3.isValid());
    //         QCOMPARE(q3.value(0).toString(), m_instance->KIND_CLICK);
    //         QCOMPARE(q3.value(1).toString(), target->name());
    //         QCOMPARE(q3.value(2).toString(), target->localVersion());
    //         QCOMPARE(q3.value(3).toString(), target->remoteVersion());
    //         QCOMPARE(q3.value(4).toInt(), target->revision());
    //         QCOMPARE(q3.value(5).toString(), QString("pending"));
    //         QCOMPARE(q3.value(8).toString(), target->title());
    //         QCOMPARE(q3.value(9).toString(), target->downloadSha512());
    //         QCOMPARE(q3.value(10).toUInt(), target->binaryFilesize());
    //         QCOMPARE(q3.value(11).toString(), target->iconUrl());
    //         QCOMPARE(q3.value(12).toString(), target->downloadUrl());
    //         QCOMPARE(q3.value(13).toString(), target->command().join(" "));
    //         QCOMPARE(q3.value(14).toString(), target->changelog());
    //         QCOMPARE(q3.value(15).toString(), target->token());
    //         QCOMPARE(q3.value(16).toString(), QString("available"));
    //         QCOMPARE(q3.value(17).toBool(), false);
    //         size++;
    //     }

    //     QCOMPARE(size, 2);

    //     // Mark as installed
    //     m_instance->setDownloadId(m.name(), m.revision(), "1234");
    //     m_instance->setInstalled("1234");

    //     size = 0;
    //     QSqlQuery q4 = m_instance->db().exec("SELECT * FROM updates WHERE state='installed'");

    //     while (q4.next()) {
    //         QVERIFY(q4.isValid());
    //         QCOMPARE(q4.value(0).toString(), m_instance->KIND_CLICK);
    //         QCOMPARE(q4.value(1).toString(), m.name());
    //         QCOMPARE(q4.value(2).toString(), m.localVersion());
    //         QCOMPARE(q4.value(3).toString(), m.remoteVersion());
    //         QCOMPARE(q4.value(4).toInt(), m.revision());
    //         QCOMPARE(q4.value(5).toString(), QString("installed"));
    //         QCOMPARE(q4.value(8).toString(), m.title());
    //         QCOMPARE(q4.value(9).toString(), m.downloadSha512());
    //         QCOMPARE(q4.value(10).toUInt(), m.binaryFilesize());
    //         QCOMPARE(q4.value(11).toString(), m.iconUrl());
    //         QCOMPARE(q4.value(12).toString(), m.downloadUrl());
    //         QCOMPARE(q4.value(13).toString(), m.command().join(" "));
    //         QCOMPARE(q4.value(14).toString(), m.changelog());
    //         QCOMPARE(q4.value(15).toString(), m.token());
    //         QCOMPARE(q4.value(16).toString(), QString("installfinished"));
    //         QCOMPARE(q4.value(17).toBool(), false);
    //         size++;
    //     }

    //     QCOMPARE(size, 1);

    //     size = 0;
    //     QSqlQuery q5 = m_instance->db().exec("SELECT * FROM updates WHERE state='pending'");

    //     while (q5.next()) {
    //         QVERIFY(q5.isValid());
    //         QCOMPARE(q5.value(0).toString(), m_instance->KIND_CLICK);
    //         QCOMPARE(q5.value(1).toString(), m2.name());
    //         QCOMPARE(q5.value(2).toString(), m2.localVersion());
    //         QCOMPARE(q5.value(3).toString(), m2.remoteVersion());
    //         QCOMPARE(q5.value(4).toInt(), m2.revision());
    //         QCOMPARE(q5.value(5).toString(), QString("pending"));
    //         QCOMPARE(q5.value(8).toString(), m2.title());
    //         QCOMPARE(q5.value(9).toString(), m2.downloadSha512());
    //         QCOMPARE(q5.value(10).toUInt(), m2.binaryFilesize());
    //         QCOMPARE(q5.value(11).toString(), m2.iconUrl());
    //         QCOMPARE(q5.value(12).toString(), m2.downloadUrl());
    //         QCOMPARE(q5.value(13).toString(), m2.command().join(" "));
    //         QCOMPARE(q5.value(14).toString(), m2.changelog());
    //         QCOMPARE(q5.value(15).toString(), m2.token());
    //         QCOMPARE(q5.value(16).toString(), QString("available"));
    //         QCOMPARE(q5.value(17).toBool(), false);
    //         size++;
    //     }

    //     QCOMPARE(size, 1);
    // }
    void testPruning()
    {
        QSharedPointer<Update> recentUpdate = createUpdate();
        recentUpdate->setIdentifier("new.app");
        recentUpdate->setRevision(1);

        QSharedPointer<Update> oldUpdate = createUpdate();
        oldUpdate->setIdentifier("old.app");
        oldUpdate->setRevision(1);

        m_instance->add(recentUpdate);
        m_instance->add(oldUpdate);

        // Change update date directly in the db
        QSqlQuery q(m_instance->db());
        q.prepare("UPDATE updates SET updated_at_utc=:updated WHERE id=:id");
        QDateTime longAgo = QDateTime::currentDateTime().addMonths(-1).addDays(-1).toUTC();

        q.bindValue(":updated", longAgo.toMSecsSinceEpoch());
        q.bindValue(":id", oldUpdate->identifier());
        q.exec();

        m_instance->pruneDb();

        QSqlQuery q1(m_instance->db());
        q1.exec("SELECT * FROM updates");
        int size = 0;
        while (q1.next()) {
            QVERIFY(q1.isValid());
            QCOMPARE(q1.value("id").toString(), recentUpdate->identifier());
            size++;
        }
        QCOMPARE(size, 1);
    }
    // void testSystemUpdate()
    // {
    //     // We basically add system updates using a freehand API,
    //     // since version information can change from channel to
    //     // channel. We test that here.
    //     QString kind("system");
    //     QString uniqueIdentifier("_ubuntu");
    //     int revision(1);
    //     QString version("OTA-42");
    //     QString changelog("Changes");
    //     QString title("Ubuntu Touch");
    //     QString iconUrl("distributor-logo.svg");
    //     int binarySize(1000);
    //     m_instance->add(kind, uniqueIdentifier, revision, version, changelog,
    //                     title, iconUrl, binarySize, false);

    //     QSqlQuery q(m_instance->db());
    //     q.exec("SELECT * FROM updates");
    //     int size = 0;
    //     while (q.next()) {
    //         QVERIFY(q.isValid());
    //         QCOMPARE(q.value(1).toString(), uniqueIdentifier);
    //         size++;
    //     }
    //     QCOMPARE(size, 1);
    // }
    // void testChangeSignal()
    // {
    //     Update app;
    //     app.setName("new.app");
    //     app.setRevision(1);

    //     QSignalSpy storeChangedSpy(m_instance, SIGNAL(changed()));
    //     m_instance->add(&app);
    //     QTRY_COMPARE(storeChangedSpy.count(), 1);

    // }
    // void testUpdateChangeSignal()
    // {
    //     Update app;
    //     app.setName("new.app");
    //     app.setRevision(1);

    //     QSignalSpy storeChangedSpy(m_instance, SIGNAL(changed()));
    //     m_instance->add(&app);
    //     QTRY_COMPARE(storeChangedSpy.count(), 1);
    // }
    void testInsertRows()
    {
        // Update upd;
        // upd.setName("new.app");
        // upd.setRevision(1);

        // QSignalSpy beginInsertSpy(m_instance, SIGNAL(beginInsertRows(QModelIndex*, int, int)));
        // QSignalSpy endInsertSpy(m_instance, SIGNAL(endInsertRows()));

        // m_instance->(&upd);
        // m_instance->refresh();

        // QTRY_COMPARE(beginInsertSpy.count(), 1);
        // QList<QVariant> arguments = beginInsertSpy.takeFirst()

        // QCOMPARE(arguments.at(1), 0);
        // QCOMPARE(arguments.at(2), 1);
        // QTRY_COMPARE(endInsertSpy.count(), 1);
    }
private:
    UpdateDb *m_instance;
    QTemporaryDir *m_dir;
    QString m_dbfile;
};

QTEST_MAIN(TstUpdateDb)

#include "tst_updatedb.moc"
