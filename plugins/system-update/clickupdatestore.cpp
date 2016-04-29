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
        m_installedUpdates(this),
        m_activeUpdates(this),
        m_dbpath("")
{
    QString dataPath = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);
    if (Q_UNLIKELY(!QDir().mkpath(dataPath))) {
        qCritical() << "Could not create" << dataPath;
        return;
    }
    m_dbpath = dataPath + QLatin1String("/clickupdatestore.db");

    initializeStore();
}

ClickUpdateStore::ClickUpdateStore(const QString &dbpath, QObject *parent) :
        QObject(parent),
        m_installedUpdates(this),
        m_activeUpdates(this),
        m_dbpath(dbpath)
{
    initializeStore();
}

ClickUpdateStore::~ClickUpdateStore()
{
    m_db.close();
    QString name = m_db.connectionName();
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(name);
}

void ClickUpdateStore::initializeStore()
{
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

    queryAll();
}

void ClickUpdateStore::add(const ClickUpdateMetadata *meta)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("INSERT OR REPLACE INTO updates (app_id, revision, state,"
              "created_at_utc, download_sha512, title, size, icon_url,"
              "download_url, changelog, click_token,"
              "local_version, remote_version) VALUES (:app_id,"
              ":revision, :state, :created_at_utc, :download_sha512, :title,"
              ":size, :icon_url, :download_url, :changelog,"
              ":click_token, :local_version, :remote_version)");
    q.bindValue(":app_id", meta->name());
    q.bindValue(":revision", meta->revision());
    q.bindValue(":state", "pending");
    q.bindValue(":created_at_utc",
                QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch());
    q.bindValue(":download_sha512", meta->downloadSha512());
    q.bindValue(":title", meta->title());
    q.bindValue(":size", meta->binaryFilesize());
    q.bindValue(":icon_url", meta->iconUrl());
    q.bindValue(":download_url", meta->downloadUrl());
    q.bindValue(":changelog", meta->changelog());
    q.bindValue(":click_token", meta->clickToken());
    q.bindValue(":local_version", meta->localVersion());
    q.bindValue(":remote_version", meta->remoteVersion());

    if (!q.exec())
        qCritical() << "Could not add click update" << q.lastError().text();

    queryActive();
}

void ClickUpdateStore::setUdmId(const QString &appId, const int &revision,
                                const int &udmId)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET udm_download_id=:udm_download_id"
              " WHERE app_id=:app_id AND revision=:revision");
    q.bindValue(":udm_download_id", udmId);
    q.bindValue(":app_id", appId);
    q.bindValue(":revision", revision);

    if (!q.exec())
        qCritical() << "could not set udm id" << appId
                    << "as installed" << q.lastError().text();

    queryActive();
}

void ClickUpdateStore::unsetUdmId(const QString &appId, const int &revision)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET udm_download_id=NULL"
              " WHERE app_id=:app_id AND revision=:revision");
    // q.bindValue(":udm_download_id", QVariant(NULL));
    q.bindValue(":app_id", appId);
    q.bindValue(":revision", revision);

    if (!q.exec())
        qCritical() << "could not unset udm id" << appId
                    << "as installed" << q.lastError().text();

    queryActive();
}

void ClickUpdateStore::queryActive()
{
    if (!openDb()) return;

    // TODO: this should query for DISTINCT app_id
    m_activeUpdates.setQuery("SELECT * FROM updates WHERE state='pending' "
                             "ORDER BY title ASC",
                             m_db);
    if (m_activeUpdates.lastError().isValid())
        qWarning() << m_activeUpdates.lastError();

    m_db.close();
}

void ClickUpdateStore::queryInstalled()
{
    if (!openDb()) return;

    m_installedUpdates.setQuery("SELECT * FROM updates WHERE state='installed' "
                                "ORDER BY updated_at_utc DESC",
                                m_db);
    if (m_installedUpdates.lastError().isValid())
        qWarning() << m_installedUpdates.lastError();

    m_db.close();
}

void ClickUpdateStore::queryAll()
{
    queryActive();
    queryInstalled();
}

void ClickUpdateStore::markInstalled(const QString &appId, const int &revision)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET state=:state, updated_at_utc=:updated_at_utc"
              " WHERE app_id=:app_id AND revision=:revision");
    q.bindValue(":state", "installed");
    q.bindValue(":updated_at_utc",
                QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch());
    q.bindValue(":app_id", appId);
    q.bindValue(":revision", revision);

    if (!q.exec())
        qCritical() << "could not mark app" << appId
                    << "as installed" << q.lastError().text();

    queryAll();
}

bool ClickUpdateStore::createDb()
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
                "app_id TEXT NOT NULL,"
                "local_version TEXT NOT NULL,"
                "remote_version TEXT NOT NULL,"
                "revision INTEGER NOT NULL,"
                "state TEXT NOT NULL,"
                "created_at_utc INTEGER NOT NULL,"
                "updated_at_utc INTEGER,"
                "title TEXT NOT NULL,"
                "download_sha512 TEXT NOT NULL,"
                "size INTEGER NOT NULL,"
                "icon_url TEXT NOT NULL,"
                "download_url TEXT NOT NULL,"
                "changelog TEXT NOT NULL,"
                "udm_download_id INTEGER,"
                "click_token TEXT DEFAULT '',"
                "PRIMARY KEY (app_id, revision))");

    if (Q_UNLIKELY(!ok)) {
        m_db.rollback();
        return false;
    }

    return m_db.commit();
}

bool ClickUpdateStore::openDb()
{
    if (m_db.isOpen()) return true;
    if (Q_UNLIKELY(!m_db.open())) {
        qCritical() << "Could not open updates database:" << m_db.lastError();
        return false;
    }
    return true;
}

void ClickUpdateStore::pruneDb()
{

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
    if (!openDb()) return d;

    QSqlQuery q(m_db);
    q.exec("SELECT checked_at_utc FROM meta ORDER BY checked_at_utc DESC;");

    if (q.next())
        d = QDateTime::fromMSecsSinceEpoch(q.value(0).toLongLong());

    m_db.close();

    return d.toUTC();
}

void ClickUpdateStore::setLastCheckDate(const QDateTime &lastCheckUtc)
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
