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
    // void testUpdateLifecycle()
    // {
    //     QSharedPointer<Update> m = QSharedPointer<Update>(new Update);
    //     m->setRevision(42);
    //     m->setIdentifier("com.ubuntu.testapp");
    //     m->setTitle("ABC");
    //     m->setToken("token");

    //     QSharedPointer<Update> m2 = QSharedPointer<Update>(new Update);
    //     m2->setRevision(100);
    //     m2->setIdentifier("com.ubuntu.myapp");
    //     m2->setTitle("XYZ");
    //     m2->setToken("token");

    //     // Add an app
    //     m_instance->add(m);

    //     QCOMPARE(m_instance->updates((uint) UpdateModel::Filter::All).size(), 1);

    //     // We had to refresh tokens, so we re-add the update.
    //     m->setToken("newtoken");
    //     m_instance->add(m);
    //     QList<QSharedPointer<Update> > afterNewTokenList = m_instance->updates((uint) UpdateModel::Filter::Pending);

    //     QCOMPARE(afterNewTokenList.size(), 1);
    //     QCOMPARE(afterNewTokenList.at(0)->token(), m->token());

    //     // Add second app
    //     m_instance->add(m2);

    //     QList<QSharedPointer<Update> > list = m_instance->updates((uint) UpdateModel::Filter::Pending);
    //     QCOMPARE(list.size(), 2);

    //     // Update titles are sorted ASC, so we'll assume that order here.
    //     QCOMPARE(list.at(0)->identifier(), m->identifier());
    //     QCOMPARE(list.at(1)->identifier(), m2->identifier());

    //     // Mark as installed
    //     m_instance->setInstalled(m2->identifier(), m2->revision());
    //     QCOMPARE(m_instance->updates((uint) UpdateModel::Filter::Pending).size(), 1);
    //     QCOMPARE(m_instance->updates((uint) UpdateModel::Filter::Installed).size(), 1);
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
private:
    UpdateDb *m_instance = nullptr;
    QString m_dbfile;
};

QTEST_MAIN(TstUpdateDb)
#include "tst_updatedb.moc"
