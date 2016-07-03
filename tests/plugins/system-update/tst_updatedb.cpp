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
#include <QTime>
#include <QTimeZone>
#include <QTest>
#include <QDebug>

#include "update.h"
#include "updatedb.h"
#include "updatemodel.h"

using namespace UpdatePlugin;

Q_DECLARE_METATYPE(QSharedPointer<Update>)
Q_DECLARE_METATYPE(UpdateModel::Filter)

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
        auto list = m_instance->updates((uint) UpdateModel::Filter::All);
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
    void testSetInstalled()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);
        m_instance->add(m);

        QSignalSpy changedSpy(m_instance, SIGNAL(changed()));
        m_instance->setInstalled(m->identifier(), m->revision());
        QTRY_COMPARE(changedSpy.count(), 1);

        // See that the DB changed the new Update instance.
        QCOMPARE(m_instance->updates().at(0)->state(), Update::State::StateInstallFinished);
        QCOMPARE(m_instance->updates().at(0)->installed(), true);
    }
    void testSetError()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);
        m_instance->add(m);
        QSignalSpy changedSpy(m_instance, SIGNAL(changed(const QString, const uint)));

        QString error("Failure.");
        m_instance->setError(m->identifier(), m->revision(), error);

        // Assert signals from db are correct
        QTRY_COMPARE(changedSpy.count(), 1);
        QList<QVariant> args = changedSpy.takeFirst();
        QCOMPARE(args.at(0).toString(), m->identifier());
        QCOMPARE(args.at(1).toUInt(), m->revision());

        // See that the DB changed the new Update instance.
        QCOMPARE(m_instance->updates().at(0)->state(), Update::State::StateFailed);
        QCOMPARE(m_instance->updates().at(0)->error(), error);
    }
    void testSetProgress()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);
        m_instance->add(m);
        QSignalSpy changedSpy(m_instance, SIGNAL(changed(const QString, const uint)));

        int progress = 50;
        m_instance->setProgress(m->identifier(), m->revision(), progress);

        // Assert signals from db are correct
        QTRY_COMPARE(changedSpy.count(), 1);
        QList<QVariant> args = changedSpy.takeFirst();
        QCOMPARE(args.at(0).toString(), m->identifier());
        QCOMPARE(args.at(1).toUInt(), m->revision());

        // See that the DB changed the new Update instance.
        QCOMPARE(m_instance->updates().at(0)->state(), Update::State::StateDownloading);
        QCOMPARE(m_instance->updates().at(0)->progress(), progress);
    }
    void testSetStarted()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);
        m_instance->add(m);
        QSignalSpy changedSpy(m_instance, SIGNAL(changed(const QString, const uint)));

        m_instance->setStarted(m->identifier(), m->revision());

        // Assert signals from db are correct
        QTRY_COMPARE(changedSpy.count(), 1);
        QList<QVariant> args = changedSpy.takeFirst();
        QCOMPARE(args.at(0).toString(), m->identifier());
        QCOMPARE(args.at(1).toUInt(), m->revision());

        // See that the DB changed the new Update instance.
        QCOMPARE(m_instance->updates().at(0)->state(), Update::State::StateDownloading);
    }
    void testSetQueued()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);
        m_instance->add(m);
        QSignalSpy changedSpy(m_instance, SIGNAL(changed(const QString, const uint)));

        m_instance->setQueued(m->identifier(), m->revision());

        // Assert signals from db are correct
        QTRY_COMPARE(changedSpy.count(), 1);
        QList<QVariant> args = changedSpy.takeFirst();
        QCOMPARE(args.at(0).toString(), m->identifier());
        QCOMPARE(args.at(1).toUInt(), m->revision());

        // See that the DB changed the new Update instance.
        QCOMPARE(m_instance->updates().at(0)->state(), Update::State::StateQueuedForDownload);
    }
    void testSetProcessing()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);
        m_instance->add(m);
        QSignalSpy changedSpy(m_instance, SIGNAL(changed(const QString, const uint)));

        m_instance->setProcessing(m->identifier(), m->revision());

        // Assert signals from db are correct
        QTRY_COMPARE(changedSpy.count(), 1);
        QList<QVariant> args = changedSpy.takeFirst();
        QCOMPARE(args.at(0).toString(), m->identifier());
        QCOMPARE(args.at(1).toUInt(), m->revision());

        // See that the DB changed the new Update instance.
        QCOMPARE(m_instance->updates().at(0)->state(), Update::State::StateInstalling);
    }
    void testSetPaused()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);
        m_instance->add(m);
        QSignalSpy changedSpy(m_instance, SIGNAL(changed(const QString, const uint)));

        m_instance->setPaused(m->identifier(), m->revision());

        // Assert signals from db are correct
        QTRY_COMPARE(changedSpy.count(), 1);
        QList<QVariant> args = changedSpy.takeFirst();
        QCOMPARE(args.at(0).toString(), m->identifier());
        QCOMPARE(args.at(1).toUInt(), m->revision());

        // See that the DB changed the new Update instance.
        QCOMPARE(m_instance->updates().at(0)->state(), Update::State::StateDownloadPaused);
    }
    void testSetResumed()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);
        m_instance->add(m);
        QSignalSpy changedSpy(m_instance, SIGNAL(changed(const QString, const uint)));

        m_instance->setResumed(m->identifier(), m->revision());

        // Assert signals from db are correct
        QTRY_COMPARE(changedSpy.count(), 1);
        QList<QVariant> args = changedSpy.takeFirst();
        QCOMPARE(args.at(0).toString(), m->identifier());
        QCOMPARE(args.at(1).toUInt(), m->revision());

        // See that the DB changed the new Update instance.
        QCOMPARE(m_instance->updates().at(0)->state(), Update::State::StateDownloading);
    }
    void testSetCanceled()
    {
        QSharedPointer<Update> m = createUpdate();
        m->setIdentifier("test.app");
        m->setRevision(1);
        m_instance->add(m);
        QSignalSpy changedSpy(m_instance, SIGNAL(changed(const QString, const uint)));

        m_instance->setCanceled(m->identifier(), m->revision());

        // Assert signals from db are correct
        QTRY_COMPARE(changedSpy.count(), 1);
        QList<QVariant> args = changedSpy.takeFirst();
        QCOMPARE(args.at(0).toString(), m->identifier());
        QCOMPARE(args.at(1).toUInt(), m->revision());

        // See that the DB changed the new Update instance.
        QCOMPARE(m_instance->updates().at(0)->state(), Update::State::StateAvailable);
    }
    void testFilters_data()
    {
        QTest::addColumn<QSharedPointer<Update> >("a");
        QTest::addColumn<QSharedPointer<Update> >("b");
        QTest::addColumn<QSharedPointer<Update> >("c");
        QTest::addColumn<QSharedPointer<Update> >("d");
        QTest::addColumn<UpdateModel::Filter>("filter");
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
        QTest::newRow("All") << a << b << c << d << UpdateModel::Filter::All << all;

        QStringList pending; pending << "a" << "b";
        QTest::newRow("Pending") << a << b << c << d << UpdateModel::Filter::Pending << pending;

        QStringList pendingClicks; pendingClicks << "a";
        QTest::newRow("PendingClicks") << a << b << c << d << UpdateModel::Filter::PendingClicks << pendingClicks;

        QStringList pendingImage; pendingImage << "b";
        QTest::newRow("PendingImage") << a << b << c << d << UpdateModel::Filter::PendingImage << pendingImage;

        QStringList installedClicks; installedClicks << "c";
        QTest::newRow("InstalledClicks") << a << b << c << d << UpdateModel::Filter::InstalledClicks << installedClicks;

        QStringList installedImage; installedImage << "d";
        QTest::newRow("InstalledImage") << a << b << c << d << UpdateModel::Filter::InstalledImage << installedImage;

        QStringList installed; installed << "c" << "d";
        QTest::newRow("Installed") << a << b << c << d << UpdateModel::Filter::Installed << installed;
    }
    void testFilters()
    {
        QFETCH(QSharedPointer<Update>, a);
        QFETCH(QSharedPointer<Update>, b);
        QFETCH(QSharedPointer<Update>, c);
        QFETCH(QSharedPointer<Update>, d);
        QFETCH(UpdateModel::Filter, filter);
        QFETCH(QStringList, ids);

        m_instance->add(a);
        m_instance->add(b);
        m_instance->add(c);
        m_instance->add(d);

        QList<QSharedPointer<Update> > updates = m_instance->updates((uint) filter);

        QStringList actualIds;
        for (int i = 0; i < updates.size(); i++) {
            actualIds << updates.at(i)->identifier();
        }

        QCOMPARE(actualIds, ids);
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
        QList<QSharedPointer<Update> > list = m_instance->updates((uint) UpdateModel::Filter::PendingClicks);
        QCOMPARE(list.count(), 1);
    }
    void testLastCheck_data() {
        QTest::addColumn<QDateTime>("set");
        QTest::addColumn<QDateTime>("target");

        QDateTime otherTz(QDate(2016, 2, 29), QTime(23, 0), QTimeZone(5 * 3600));
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
    void testUpdateLifecycle()
    {
        QSharedPointer<Update> m = QSharedPointer<Update>(new Update);
        m->setRevision(42);
        m->setIdentifier("com.ubuntu.testapp");
        m->setTitle("ABC");
        m->setToken("token");

        QSharedPointer<Update> m2 = QSharedPointer<Update>(new Update);
        m2->setRevision(100);
        m2->setIdentifier("com.ubuntu.myapp");
        m2->setTitle("XYZ");
        m2->setToken("token");

        // Add an app
        m_instance->add(m);

        QCOMPARE(m_instance->updates((uint) UpdateModel::Filter::All).size(), 1);

        // We had to refresh tokens, so we re-add the update.
        m->setToken("newtoken");
        m_instance->add(m);
        QList<QSharedPointer<Update> > afterNewTokenList = m_instance->updates((uint) UpdateModel::Filter::Pending);

        QCOMPARE(afterNewTokenList.size(), 1);
        QCOMPARE(afterNewTokenList.at(0)->token(), m->token());

        // Add second app
        m_instance->add(m2);

        QList<QSharedPointer<Update> > list = m_instance->updates((uint) UpdateModel::Filter::Pending);
        QCOMPARE(list.size(), 2);

        // Update titles are sorted ASC, so we'll assume that order here.
        QCOMPARE(list.at(0)->identifier(), m->identifier());
        QCOMPARE(list.at(1)->identifier(), m2->identifier());

        // Mark as installed
        m_instance->setInstalled(m2->identifier(), m2->revision());
        QCOMPARE(m_instance->updates((uint) UpdateModel::Filter::Pending).size(), 1);
        QCOMPARE(m_instance->updates((uint) UpdateModel::Filter::Installed).size(), 1);
    }
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
    void testNonExistingUpdates()
    {
        m_instance->setInstalled("not there", 0);
        m_instance->setError("not there", 0, "nope");
        m_instance->setProgress("not there", 0, 0);
        m_instance->setStarted("not there", 0);
        m_instance->setQueued("not there", 0);
        m_instance->setProcessing("not there", 0);
        m_instance->setPaused("not there", 0);
        m_instance->setResumed("not there", 0);
        m_instance->setCanceled("not there", 0);


        QSqlQuery q(m_instance->db());
        q.exec("SELECT * FROM updates");
        QVERIFY(!q.next());

    }
private:
    UpdateDb *m_instance;
    QString m_dbfile;
};

QTEST_MAIN(TstUpdateDb)

#include "tst_updatedb.moc"
