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
#include <QDebug>
#include <QScopedPointer>
#include <QSignalSpy>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QTest>
#include <QTime>
#include <QTimeZone>

#include "update.h"
#include "updatedb.h"

using namespace UpdatePlugin;

Q_DECLARE_METATYPE(QSharedPointer<Update>)

class TstUpdateDb : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_instance = new UpdateDb(":memory:");
    }
    void cleanup()
    {
        QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        m_instance->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);

    }
    QSharedPointer<Update> createUpdate()
    {
        return QSharedPointer<Update>(new Update);
    }
    void testNoUpdates()
    {
        auto list = m_instance->updates();
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

        auto list = m_instance->updates();
        QCOMPARE(list.size(), 0);
    }
    void testGetUpdate()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);
        m_instance->add(m);

        QSharedPointer<Update> m1 = m_instance->get(m->identifier(), m->revision());
        QCOMPARE(m->identifier(), m1->identifier());
        QCOMPARE(m->revision(), m1->revision());
    }
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
        QList<QSharedPointer<Update> > list = m_instance->updates();
        QCOMPARE(list.count(), 1);
        QCOMPARE(list.at(0)->revision(), replacement->revision());
    }
    void testInstalledUpdatesAreNotSuperseded()
    {
        QSharedPointer<Update> installed = createUpdate();
        installed->setIdentifier("some.app");
        installed->setRevision(1);
        installed->setKind(Update::Kind::KindClick);
        installed->setInstalled(true);

        QSharedPointer<Update> next = createUpdate();
        next->setIdentifier("some.app");
        next->setRevision(2);
        next->setKind(Update::Kind::KindClick);

        m_instance->add(installed);
        m_instance->add(next);

        // We want to keep both.
        QList<QSharedPointer<Update> > list = m_instance->updates();
        QCOMPARE(list.count(), 2);
    }
    void testLastCheck_data() {
        QTest::addColumn<QDateTime>("set");
        QTest::addColumn<QDateTime>("target");

        QTimeZone otherTz("UTC+05:00");
        QDateTime otherTzDt(QDate(2016, 2, 29), QTime(23, 0), otherTz);
        QDateTime utcTz(QDate(2016, 2, 29), QTime(18, 0), Qt::UTC);

        if (otherTzDt.toUTC() != utcTz.toUTC())
            QSKIP("Non-UTC timezones are not supported.");

        QTest::newRow("Different TZ") << otherTzDt << utcTz;

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
    void testPruning()
    {
        QSharedPointer<Update> recentUpdate = createUpdate();
        recentUpdate->setIdentifier("new.app");
        recentUpdate->setRevision(1);
        recentUpdate->setInstalled(true);
        recentUpdate->setUpdatedAt(QDateTime::currentDateTime().addDays(-1).toUTC());

        QSharedPointer<Update> oldUpdate = createUpdate();
        oldUpdate->setIdentifier("old.app");
        oldUpdate->setRevision(1);
        oldUpdate->setInstalled(true);
        oldUpdate->setUpdatedAt(QDateTime::currentDateTime().addMonths(-1).addDays(-1).toUTC());

        m_instance->add(recentUpdate);
        m_instance->add(oldUpdate);

        m_instance->pruneDb();

        QCOMPARE(m_instance->updates().size(), 1);
    }
    void testUpdateAfterInstalled()
    {
        QSharedPointer<Update> installed = QSharedPointer<Update>(new Update);
        installed->setRevision(100);
        installed->setIdentifier("com.ubuntu.myapp");
        installed->setInstalled(true);
        m_instance->add(installed);

        // Add a new revision of it.
        installed->setRevision(101);
        m_instance->add(installed);

        QCOMPARE(m_instance->updates().size(), 2);
    }
    void testUpdatedAt()
    {
        // Ensures that updatedAt comes back invalid if not updated.
        auto update = QSharedPointer<Update>(new Update);
        update->setRevision(100);
        update->setIdentifier("com.ubuntu.myapp");
        m_instance->add(update);

        auto update1 = m_instance->get(update->identifier(),
                                       update->revision());
        QCOMPARE(QDateTime(), update1->updatedAt());
    }
    void testEnterAndRestore()
    {
        auto date = QDateTime(QDate(2016, 2, 29), QTime(18, 0), Qt::UTC);
        auto update = QSharedPointer<Update>(new Update);

        update->setKind(Update::Kind::KindClick);
        update->setIdentifier("a");
        update->setRevision(1);
        update->setBinaryFilesize(100);
        update->setChangelog("changes");
        update->setChannel("");
        update->setCreatedAt(date);
        update->setUpdatedAt(date);
        update->setDownloadHash("hash");
        update->setDownloadId("did");
        update->setDownloadUrl("durl");
        update->setIconUrl("iurl");
        update->setInstalled(true);
        update->setProgress(42);
        update->setState(Update::State::StateDownloading);
        update->setSignedDownloadUrl("signedurl");
        update->setTitle("title");
        update->setLocalVersion("v1");
        update->setRemoteVersion("v2");
        update->setToken("token");

        QStringList cmd; cmd << "ls" << "-la";
        update->setCommand(cmd);
        update->setAutomatic(true);
        update->setError("error");
        update->setPackageName("packagename");

        m_instance->add(update);
        auto dbUpdate = m_instance->updates()[0];

        QCOMPARE(dbUpdate->kind(), Update::Kind::KindClick);
        QCOMPARE(dbUpdate->identifier(), QString("a"));
        QCOMPARE(dbUpdate->revision(), (uint) 1);
        QCOMPARE(dbUpdate->binaryFilesize(), (uint) 100);
        QCOMPARE(dbUpdate->changelog(), QString("changes"));
        QCOMPARE(dbUpdate->channel(),QString( ""));
        QCOMPARE(dbUpdate->updatedAt(), date);
        QCOMPARE(dbUpdate->downloadHash(), QString("hash"));
        QCOMPARE(dbUpdate->downloadId(), QString("did"));
        QCOMPARE(dbUpdate->downloadUrl(), QString("durl"));
        QCOMPARE(dbUpdate->iconUrl(), QString("iurl"));
        QCOMPARE(dbUpdate->installed(), true);
        QCOMPARE(dbUpdate->progress(), 42);
        QCOMPARE(dbUpdate->state(), Update::State::StateDownloading);
        QCOMPARE(dbUpdate->signedDownloadUrl(), QString("signedurl"));
        QCOMPARE(dbUpdate->title(), QString("title"));
        QCOMPARE(dbUpdate->localVersion(), QString("v1"));
        QCOMPARE(dbUpdate->remoteVersion(), QString("v2"));
        QCOMPARE(dbUpdate->token(), QString("token"));
        QCOMPARE(dbUpdate->command(), cmd);
        QCOMPARE(dbUpdate->automatic(), true);
        QCOMPARE(dbUpdate->error(), QString("error"));
        QCOMPARE(dbUpdate->packageName(), QString("packagename"));
    }
    void testSchemaVersion()
    {
        QSqlQuery q(m_instance->db());
        q.exec("SELECT schema_version FROM meta");
        QVERIFY(q.next());
        QCOMPARE(q.value("schema_version").toUInt(), m_instance->SCHEMA_VERSION);
    }
    void testMigration()
    {
        QTemporaryDir dir;
        // “Migration”: We nuke the DB if we get an unexpected schema version.
        auto dbPath = QString("%1/%2").arg(dir.path()).arg("migrationtest.db");
        qDebug() << "created temp path at" << dbPath;
        auto instance = new UpdateDb(dbPath);

        // Add an update so we can confirm a migration happened.
        auto update = createUpdate();
        update->setIdentifier("test");
        update->setRevision(1);
        instance->add(update);

        // We're now at version SCHEMA_VERSION. Decrease it to force migration.
        QSqlQuery q(instance->db());
        q.prepare("UPDATE meta SET schema_version=:schema_version");
        q.bindValue(":schema_version", instance->SCHEMA_VERSION - 1);
        QVERIFY(q.exec());
        delete instance;

        instance = new UpdateDb(dbPath);
        // Confirm that the DB was nuked by asserting count is 0.
        QCOMPARE(instance->updates().count(), 0);
        instance->deleteLater();
    }
private:
    UpdateDb *m_instance = nullptr;
};

QTEST_GUILESS_MAIN(TstUpdateDb)
#include "tst_updatedb.moc"
