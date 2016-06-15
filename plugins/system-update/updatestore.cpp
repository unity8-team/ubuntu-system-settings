/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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

#include "updatestore.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

namespace UpdatePlugin
{

UpdateStore::UpdateStore(QObject *parent)
    : QObject(parent)
    , m_dbpath("")
{
    QString dataPath = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);
    if (Q_UNLIKELY(!QDir().mkpath(dataPath))) {
        qCritical() << "Could not create" << dataPath;
        return;
    }
    m_dbpath = dataPath + QLatin1String("/updatestore.db");

    initializeStore();
}

UpdateStore::UpdateStore(const QString &dbpath, QObject *parent)
    : QObject(parent)
    , m_dbpath(dbpath)
{
    initializeStore();
}

UpdateStore::~UpdateStore()
{
    m_db.close();
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(m_connectionName);
}

void UpdateStore::initializeStore()
{
    // Create a unique connection name
    int connI = 0;
    while (m_connectionName.isEmpty()) {
        QString tmpl("update-store-%1");
        if (!QSqlDatabase::contains(tmpl.arg(connI)))
            m_connectionName = tmpl.arg(connI);
        connI++;
    }

    m_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), m_connectionName);
    m_db.setDatabaseName(m_dbpath);
    if (!openDb()) return;
    QSqlQuery q(m_db);

    // Check whether the table already exists
    q.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='updates'");
    if (!q.next() && !createDb()) {
        qCritical() << "Could not create updates database:" << m_dbpath
            << m_db.lastError().text();
        return;
    }

    m_db.close();
}

void UpdateStore::add(const ClickUpdateMetadata *meta)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("INSERT OR REPLACE INTO updates (app_id, revision, state,"
              "created_at_utc, download_sha512, title, size, icon_url,"
              "download_url, changelog, command, click_token,"
              "local_version, remote_version, kind) VALUES (:app_id,"
              ":revision, :state, :created_at_utc, :download_sha512, :title,"
              ":size, :icon_url, :download_url, :changelog, :command,"
              ":click_token, :local_version, :remote_version, :kind)");
    q.bindValue(":app_id", meta->name());
    q.bindValue(":revision", meta->revision());
    q.bindValue(":state", "pending");
    q.bindValue(":created_at_utc",
                QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch());;
    q.bindValue(":download_sha512", meta->downloadSha512());
    q.bindValue(":title", meta->title());
    q.bindValue(":size", meta->binaryFilesize());
    q.bindValue(":icon_url", meta->iconUrl());
    q.bindValue(":download_url", meta->downloadUrl());
    q.bindValue(":changelog", meta->changelog());
    q.bindValue(":command", meta->command().join(" "));
    q.bindValue(":click_token", meta->clickToken());
    q.bindValue(":local_version", meta->localVersion());
    q.bindValue(":remote_version", meta->remoteVersion());
    q.bindValue(":kind", KIND_CLICK);

    if (!q.exec())
        qCritical() << "Could not add click update" << q.lastError().text();

    m_db.close();

    Q_EMIT (updatesChanged());
}

void UpdateStore::add(const QString &kind, const QString &uniqueIdentifier,
                      const int &revision, const QString &version,
                      const QString &changelog, const QString &title,
                      const QString &iconUrl, const int &size)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("INSERT OR REPLACE INTO updates (app_id, revision, state,"
              "created_at_utc, title, size, icon_url, changelog,"
              "remote_version, kind) VALUES (:app_id,"
              ":revision, :state, :created_at_utc, :title,"
              ":size, :icon_url, :changelog, :remote_version, :kind)");
    q.bindValue(":app_id", uniqueIdentifier);
    q.bindValue(":revision", revision);
    q.bindValue(":state", "pending");
    q.bindValue(":created_at_utc",
                QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch());;
    q.bindValue(":title", title);
    q.bindValue(":size", size);
    q.bindValue(":icon_url", iconUrl);
    q.bindValue(":changelog", changelog);
    q.bindValue(":remote_version", version);
    q.bindValue(":kind", kind);

    if (!q.exec())
        qCritical() << "Could not add update" << q.lastError().text();

    m_db.close();

    Q_EMIT (updatesChanged());
}

QSqlDatabase UpdateStore::db() const
{
    return m_db;
}

void UpdateStore::markInstalled(const QString &uniqueIdentifier, const int &revision)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET state=:state, updated_at_utc=:updated_at_utc"
              " WHERE app_id=:app_id AND revision=:revision");
    q.bindValue(":state", "installed");
    q.bindValue(":updated_at_utc",
                QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch());
    q.bindValue(":app_id", uniqueIdentifier);
    q.bindValue(":revision", revision);

    if (!q.exec())
        qCritical() << "could not mark app" << uniqueIdentifier
                    << "as installed" << q.lastError().text();

    m_db.close();

    Q_EMIT (updatesChanged());
}

bool UpdateStore::createDb()
{
    if (Q_UNLIKELY(!m_db.transaction())) return false;

    QSqlQuery q(m_db);
    bool ok;
    ok = q.exec("CREATE TABLE meta(checked_at_utc BIGINT)");
    if (Q_UNLIKELY(!ok)) {
        m_db.rollback();
        return false;
    }

    ok = q.exec("CREATE TABLE updates ("
                "kind TEXT NOT NULL,"
                "app_id TEXT NOT NULL,"
                "local_version TEXT,"
                "remote_version TEXT,"
                "revision INTEGER NOT NULL,"
                "state TEXT NOT NULL,"
                "created_at_utc BIGINT NOT NULL,"
                "updated_at_utc BIGINT,"
                "title TEXT NOT NULL,"
                "download_sha512 TEXT,"
                "size INTEGER NOT NULL,"
                "icon_url TEXT NOT NULL,"
                "download_url TEXT,"
                "command TEXT,"
                "changelog TEXT,"
                "click_token TEXT DEFAULT '',"
                "PRIMARY KEY (app_id, revision))");

    if (Q_UNLIKELY(!ok)) {
        m_db.rollback();
        return false;
    }

    return m_db.commit();
}

bool UpdateStore::openDb()
{
    if (m_db.isOpen()) return true;
    if (Q_UNLIKELY(!m_db.open())) {
        qCritical() << "Could not open updates database:" << m_db.lastError();
        return false;
    }
    return true;
}

void UpdateStore::pruneDb()
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    QDateTime monthAgo = QDateTime::currentDateTime().addMonths(-1).toUTC();
    q.prepare("DELETE FROM updates WHERE updated_at_utc < :updated");
    q.bindValue(":updated", monthAgo.toMSecsSinceEpoch());

    if (!q.exec())
        qCritical() << "could not prune db" << q.lastError().text();

    m_db.close();
}

QDateTime UpdateStore::lastCheckDate()
{
    QDateTime d;
    if (!openDb()) return d;

    QSqlQuery q(m_db);
    q.exec("SELECT checked_at_utc FROM meta ORDER BY checked_at_utc DESC;");

    if (q.next())
        d = QDateTime::fromMSecsSinceEpoch(q.value(0).toLongLong());

    m_db.close();

    return d.toUTC();
}

void UpdateStore::setLastCheckDate(const QDateTime &lastCheckUtc)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("REPLACE INTO meta (checked_at_utc) VALUES (:checked_at_utc)");
    q.bindValue(":checked_at_utc", lastCheckUtc.toMSecsSinceEpoch());

    if (!q.exec())
        qCritical() << "could not update checked at value" << q.lastError().text();

    m_db.close();
}

} // UpdatePlugin
