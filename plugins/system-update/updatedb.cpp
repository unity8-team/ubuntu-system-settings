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

#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

namespace UpdatePlugin
{
namespace {
const QString ALL = "kind, id, local_version, remote_version, revision, \
    installed, created_at_utc, updated_at_utc, title, download_hash, size, \
    icon_url, download_url, command, changelog, token, download_id, \
    update_state, signed_download_url, progress, automatic, error, \
    package_name";
const QString GET_SINGLE = "SELECT " + ALL + " FROM updates WHERE id=:id \
    AND revision=:revision";
const QString GET_ALL = "SELECT " + ALL + " FROM updates";
}

UpdateDb::UpdateDb(QObject *parent)
    : QObject(parent)
    , m_dbpath("")
{
    QString dataPath = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation
    );
    if (Q_UNLIKELY(!QDir().mkpath(dataPath))) {
        qCritical() << Q_FUNC_INFO << "Could not create" << dataPath;
        return;
    }
    m_dbpath = dataPath + QLatin1String("/updatestore.db");

    initializeDb();
}

UpdateDb::UpdateDb(const QString &dbpath, QObject *parent)
    : QObject(parent)
    , m_dbpath(dbpath)
{
    initializeDb();
}

void UpdateDb::initializeDb()
{
    // Create a unique connection name
    int connI = 0;
    while (m_connectionName.isEmpty()) {
        QString tmpl("system-settings-update-%1");
        if (!QSqlDatabase::contains(tmpl.arg(connI)))
            m_connectionName = tmpl.arg(connI);
        connI++;
    }

    m_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"),
                                     m_connectionName);
    m_db.setDatabaseName(m_dbpath);

    if (!openDb()) {
        return;
    }

    QSqlQuery q(m_db);
    q.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='updates'");
    bool exists = q.next();
    q.finish();

    if (exists) {
        if (!migrateDb()) {
            qCritical() << "Unable to migrate db" << m_db.lastError().text();
            return;
        }
    } else {
        if (!createDb()) {
            qCritical() << "Unable to create db" << m_db.lastError().text();
            return;
        }
    }
}

bool UpdateDb::migrateDb()
{
    uint version = 0;
    QSqlQuery q(m_db);
    q.exec("SELECT schema_version FROM meta");
    if (q.next()) {
        version = q.value(0).toUInt();
    }
    q.finish();

    if (version != SCHEMA_VERSION) {
        if (!dropDb()) {
            qCritical() << "Unable to drop db" << m_db.lastError().text();
            return false;
        }
        if (!createDb()) {
            qCritical() << "Unable to create db" << m_db.lastError().text();
            return false;
        }
    }
    return true;
}

UpdateDb::~UpdateDb()
{
    m_db.close();
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(m_connectionName);
}

void UpdateDb::add(const QSharedPointer<Update> &update)
{
    replaceWith(update);
    if (insert(update)) {
        Q_EMIT changed();
    }
}

void UpdateDb::replaceWith(const QSharedPointer<Update> &update)
{
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM updates WHERE id=:id AND revision < :revision "
              "AND installed=:installed");
    q.bindValue(":id", update->identifier());
    q.bindValue(":revision", update->revision());
    q.bindValue(":installed", false);
    if (!q.exec()) {
        qCritical() << Q_FUNC_INFO << q.lastError().text();
    }
}

void UpdateDb::update(const QSharedPointer<Update> &update)
{
    if (insert(update)) {
        Q_EMIT changed(update);
    }
}

bool UpdateDb::insert(const QSharedPointer<Update> &update)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT OR REPLACE INTO updates (id, revision, installed,"
              "created_at_utc, download_hash, title, size, icon_url,"
              "download_url, changelog, command, token, download_id, progress,"
              "local_version, remote_version, kind, update_state, automatic,"
              "error, package_name, updated_at_utc, signed_download_url) "
              "VALUES (:id, :revision, :installed, :created_at_utc,"
              ":download_hash, :title, :size, :icon_url, :download_url,"
              ":changelog, :command, :token, :download_id, :progress,"
              ":local_version, :remote_version, :kind, :update_state,"
              ":automatic, :error, :package_name, :updated_at_utc, "
              ":signed_download_url)");
    q.bindValue(":id", update->identifier());
    q.bindValue(":revision", update->revision());
    q.bindValue(":installed", update->installed());
    q.bindValue(":created_at_utc",
                QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch());;
    q.bindValue(":download_hash", update->downloadHash());
    q.bindValue(":title", update->title());
    q.bindValue(":size", update->binaryFilesize());
    q.bindValue(":icon_url", update->iconUrl());
    q.bindValue(":download_url", update->downloadUrl());
    q.bindValue(":changelog", update->changelog());
    q.bindValue(":command", update->command().join(" "));
    q.bindValue(":token", update->token());
    q.bindValue(":download_id", update->downloadId());
    q.bindValue(":progress", update->progress());
    q.bindValue(":local_version", update->localVersion());
    q.bindValue(":remote_version", update->remoteVersion());
    q.bindValue(":kind", Update::kindToString(update->kind()));
    q.bindValue(
        ":update_state",
        Update::stateToString(update->state())
    );
    q.bindValue(":automatic", update->automatic());
    q.bindValue(":error", update->error());
    q.bindValue(":package_name", update->packageName());
    q.bindValue(":updated_at_utc", update->updatedAt().toUTC().toMSecsSinceEpoch());
    q.bindValue(":signed_download_url", update->signedDownloadUrl());

    bool success = q.exec();
    if (!success) {
        qCritical() << Q_FUNC_INFO << q.lastError().text();
    }
    return success;
}

void UpdateDb::remove(const QSharedPointer<Update> &update)
{
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM updates WHERE id=:id AND revision=:revision");
    q.bindValue(":id", update->identifier());
    q.bindValue(":revision", update->revision());
    if (!q.exec()) {
        qCritical() << Q_FUNC_INFO << q.lastError().text();
    }
    Q_EMIT changed();
}

void UpdateDb::update(const QSharedPointer<Update> &update, const QSqlQuery &query)
{
    update->setKind(Update::stringToKind(
        query.value("kind").toString()
    ));
    update->setIdentifier(query.value("id").toString());
    update->setLocalVersion(query.value("local_version").toString());
    update->setRemoteVersion(query.value("remote_version").toString());
    update->setRevision(query.value("revision").toUInt());
    update->setInstalled(query.value("installed").toBool());
    update->setCreatedAt(QDateTime::fromMSecsSinceEpoch(
        query.value("created_at_utc").toLongLong()
    ).toUTC());

    qlonglong updatedAt(query.value("updated_at_utc").toLongLong());
    if (updatedAt > 0) {
        update->setUpdatedAt(QDateTime::fromMSecsSinceEpoch(updatedAt).toUTC());
    } else {
        update->setUpdatedAt(QDateTime());
    }

    update->setTitle(query.value("title").toString());
    update->setDownloadHash(query.value("download_hash").toString());
    update->setBinaryFilesize(query.value("size").toUInt());
    update->setIconUrl(query.value("icon_url").toString());
    update->setDownloadUrl(query.value("download_url").toString());
    update->setCommand(query.value("command").toString().split(" "));
    update->setChangelog(query.value("changelog").toString());
    update->setToken(query.value("token").toString());
    update->setDownloadId(query.value("download_id").toString());
    update->setState(Update::stringToState(
        query.value("update_state").toString()
    ));
    update->setProgress(query.value("progress").toInt());
    update->setAutomatic(query.value("automatic").toBool());
    update->setError(query.value("error").toString());
    update->setPackageName(query.value("package_name").toString());
    update->setSignedDownloadUrl(
        query.value("signed_download_url").toString()
    );
}

bool UpdateDb::dropDb()
{
    QSqlQuery q(m_db);
    if (!q.exec("DROP TABLE IF EXISTS meta")) {
        qCritical() << "failed to drop table meta" << m_db.lastError();
        return false;
    }
    if (!q.exec("DROP TABLE IF EXISTS updates")) {
        qCritical() << "failed to drop table updates" << m_db.lastError();
        return false;
    }
    return true;
}

bool UpdateDb::createDb()
{
    if (Q_UNLIKELY(!m_db.transaction())) return false;

    QSqlQuery q(m_db);
    bool ok;
    ok = q.exec("CREATE TABLE meta(checked_at_utc BIGINT, "
                         "schema_version INTEGER)");
    if (Q_UNLIKELY(!ok)) {
        m_db.rollback();
        return false;
    }
    q.prepare("INSERT INTO meta (schema_version) VALUES (:schema_version)");
    q.bindValue(":schema_version", SCHEMA_VERSION);
    q.exec();

    ok = q.exec("CREATE TABLE updates ("
                "kind TEXT NOT NULL,"
                "id TEXT NOT NULL,"
                "local_version TEXT,"
                "remote_version TEXT,"
                "revision INTEGER NOT NULL,"
                "installed INTEGER NOT NULL,"
                "created_at_utc BIGINT NOT NULL,"
                "updated_at_utc BIGINT,"
                "title TEXT,"
                "download_hash TEXT,"
                "size INTEGER,"
                "icon_url TEXT,"
                "download_url TEXT,"
                "command TEXT,"
                "changelog TEXT,"
                "token TEXT,"
                "download_id TEXT,"
                "update_state TEXT DEFAULT 'unknown',"
                "signed_download_url TEXT,"
                "progress INTEGER,"
                "automatic INTEGER DEFAULT 0,"
                "error TEXT,"
                "package_name TEXT,"
                "PRIMARY KEY (id, revision))");
    if (Q_UNLIKELY(!ok)) {
        m_db.rollback();
        return false;
    }
    return m_db.commit();
}

bool UpdateDb::openDb()
{
    if (m_db.isOpen()) return true;
    if (Q_UNLIKELY(!m_db.open())) {
        qCritical() << Q_FUNC_INFO << m_db.lastError();
        return false;
    }
    return true;
}

QSqlDatabase UpdateDb::db()
{
    return m_db;
}

void UpdateDb::pruneDb()
{
    QSqlQuery q(m_db);
    QDateTime monthAgo = QDateTime::currentDateTime().addMonths(-1).toUTC();
    q.prepare("DELETE FROM updates WHERE updated_at_utc < :updated");
    q.bindValue(":updated", monthAgo.toMSecsSinceEpoch());
    if (!q.exec()) {
        qCritical() << Q_FUNC_INFO << q.lastError().text();
    }
}

void UpdateDb::reset()
{
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM updates");
    if (!q.exec()) {
        qCritical() << Q_FUNC_INFO << q.lastError().text();
    }
}

QDateTime UpdateDb::lastCheckDate()
{
    QDateTime d;
    QSqlQuery q(m_db);
    q.exec("SELECT checked_at_utc FROM meta ORDER BY checked_at_utc DESC");
    if (q.next()) {
        auto val = q.value(0).toString();
        if (val.isEmpty()) {
            d = QDateTime();
        } else {
            d = QDateTime::fromMSecsSinceEpoch(q.value(0).toLongLong());
        }
    }
    return d.toUTC();
}

void UpdateDb::setLastCheckDate(const QDateTime &lastCheck)
{
    QSqlQuery q(m_db);
    q.prepare("REPLACE INTO meta (checked_at_utc) VALUES (:checked_at_utc)");
    q.bindValue(":checked_at_utc", lastCheck.toUTC().toMSecsSinceEpoch());
    if (!q.exec()) {
        qCritical() << Q_FUNC_INFO << q.lastError().text();
    }
}

QList<QSharedPointer<Update> > UpdateDb::updates()
{
    QList<QSharedPointer<Update> > list;
    QSqlQuery q(m_db);
    q.prepare(GET_ALL);
    if (!q.exec()) {
        qCritical() << Q_FUNC_INFO << q.lastError().text();
        return list;
    }
    while (q.next()) {
        auto update = QSharedPointer<Update>(new Update);
        this->update(update, q);
        list.append(update);
    }
    return list;
}

QSharedPointer<Update> UpdateDb::get(const QString &id, const uint &revision)
{
    QSqlQuery q(m_db);
    q.prepare(GET_SINGLE);
    q.bindValue(":id", id);
    q.bindValue(":revision", revision);
    if (!q.exec()) {
        qCritical() << Q_FUNC_INFO << q.lastError().text();
    }
    if (q.next()) {
        auto update = QSharedPointer<Update>(new Update);
        this->update(update, q);
        return update;
    } else {
        return QSharedPointer<Update>(nullptr);
    }
}
} // UpdatePlugin
