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

#include "updatedb.h"
#include "testable_system_update.h"

#include <QSignalSpy>
#include <QSqlError>
#include <QSqlQuery>
#include <QTest>

using namespace UpdatePlugin;

class TstSystemUpdate : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_db = new UpdateDb(":memory:");
        m_instance = new TestableSystemUpdate(m_db, nullptr);
        m_db->setParent(m_instance);
    }
    void cleanup()
    {
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

        QSqlQuery q(m_db->db());

        q.prepare("REPLACE INTO meta (checked_at_utc) VALUES (:checked_at_utc)");
        q.bindValue(":checked_at_utc", checkedAt.toMSecsSinceEpoch());

        QVERIFY(q.exec());

        QCOMPARE(m_instance->isCheckRequired(), checkRequired);
    }
    void testDb()
    {
        QCOMPARE(m_instance->db(), m_db);
    }
    void testNam()
    {
        // We passed a nullptr, so just verify that the method exist.
        QVERIFY(!m_instance->nam());
    }
private:
    TestableSystemUpdate *m_instance = nullptr;
    UpdateDb *m_db = nullptr;
};

QTEST_MAIN(TstSystemUpdate)
#include "tst_system_update.moc"