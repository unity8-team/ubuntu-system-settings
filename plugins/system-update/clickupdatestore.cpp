/*
 * Copyright (C) 2013-2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "clickupdatestore.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

namespace UpdatePlugin
{

ClickUpdateStore::ClickUpdateStore(QObject *parent) :
        QObject(parent),
        m_db(),
        m_installedUpdates(this),
        m_activeUpdates(this)
{
    m_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
    if (!m_db.isValid())
        qWarning() << "the db wasn't valid in the initializer";
    initializeStore();
}

ClickUpdateStore::~ClickUpdateStore()
{
    m_db.close();
}

void ClickUpdateStore::initializeStore()
{
    QString dbPath =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
        + QDir::separator() + "clickupdatestore.db";
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qCritical() << "Could not open updates database:" <<
            dbPath << m_db.lastError().text();
        return;
    }

    QSqlQuery q(m_db);
    // Check whether the table already exists
    q.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='updates'");
    if (!q.next() && !createDb()) {
        qCritical() << "Could not create updates database:" << dbPath
            << m_db.lastError().text();
        return;
    }

    m_activeUpdates.setQuery("SELECT * FROM updates WHERE state='pending';",
                             m_db);
    if (m_activeUpdates.lastError().isValid())
        qWarning() << m_activeUpdates.lastError();

    m_installedUpdates.setQuery("SELECT * FROM updates WHERE "
                                "state='installed';",
                                m_db);
    if (m_installedUpdates.lastError().isValid())
        qWarning() << m_installedUpdates.lastError();
}

bool ClickUpdateStore::createDb()
{
    if (Q_UNLIKELY(!m_db.transaction())) return false;

    QSqlQuery q(m_db);
    bool ok;
    ok = q.exec("CREATE TABLE meta(checked_at_utc INTEGER)");
    if (Q_UNLIKELY(!ok)) {
        m_db.rollback();
        return false;
    }

    // There is duplication and several candidate keys in this model,
    // but we ignore it so as to keep the data simple.
    ok = q.exec("CREATE TABLE updates (id INTEGER PRIMARY KEY,"
                "app_id TEXT NOT NULL,"
                "version TEXT NOT NULL,"
                "revision INTEGER NOT NULL,"
                "state TEXT NOT NULL,"
                "created_at_utc INTEGER NOT NULL,"
                "updated_at_utc INTEGER,"
                "title TEXT NOT NULL,"
                "size INTEGER NOT NULL,"
                "icon_url TEXT NOT NULL,"
                "download_url TEXT NOT NULL,"
                "changelog TEXT NOT NULL,"
                "udm_download_id INTEGER,"
                "click_token TEXT DEFAULT '')");

    if (Q_UNLIKELY(!ok)) {
        m_db.rollback();
        return false;
    }

    return m_db.commit();
}

QSqlQueryModel *ClickUpdateStore::installedUpdates()
{
    return &m_installedUpdates;
}

QSqlQueryModel *ClickUpdateStore::activeUpdates()
{
    return &m_activeUpdates;
}

QDateTime ClickUpdateStore::lastCheckDate()
{
    QDateTime d;
    if (Q_UNLIKELY(!m_db.open())) {
        qCritical() << "Could not open updates database:" << m_db.lastError();
        return d;
    }

    QSqlQuery q(m_db);
    q.exec("SELECT checked_at_utc FROM meta;");

    if (q.next())
        d = QDateTime::fromTime_t(q.value(0).toUInt());

    m_db.close();

    return d.toUTC();
}

} // UpdatePlugin
