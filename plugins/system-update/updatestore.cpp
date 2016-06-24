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
const QString UpdateStore::KIND_CLICK = QLatin1String("click");
const QString UpdateStore::KIND_SYSTEM = QLatin1String("system");
const QString UpdateStore::STATE_PENDING = QLatin1String("pending");
const QString UpdateStore::STATE_INSTALLED = QLatin1String("installed");

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

    connect(this, &UpdateStore::changed,
            SystemUpdate::instance(), &SystemUpdate::notifyStoreChanged);
    connect(this,
            SIGNAL(itemChanged(QString, int)),
            SystemUpdate::instance(),
            SLOT(notifyStoreItemChanged(QString, int)));

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
    qWarning() << "removing database" << m_connectionName;
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
    qWarning() << "initializeStore with conn name" << m_connectionName;

    m_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), m_connectionName);
    m_db.setDatabaseName(m_dbpath);
    if (!openDb()) return;
    QSqlQuery q(m_db);

    // Check whether the table already exists
    q.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='updates'");
    if (!q.next() && !createDb()) {
        qCritical() << "Could not create updates database:" << m_dbpath
            << m_db.lastError().text();
    }
    q.finish();
}

void UpdateStore::add(const ClickUpdateMetadata *meta)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT OR REPLACE INTO updates (id, revision, state,"
              "created_at_utc, download_sha512, title, size, icon_url,"
              "download_url, changelog, command, token,"
              "local_version, remote_version, kind, update_state, automatic) "
              "VALUES (:id, :revision, :state, :created_at_utc, "
              ":download_sha512, :title, :size, :icon_url, :download_url,"
              ":changelog, :command, :token, :local_version,"
              ":remote_version, :kind, :update_state, :automatic)");
    q.bindValue(":id", meta->name());
    q.bindValue(":revision", meta->revision());
    q.bindValue(":state", UpdateStore::STATE_PENDING);
    q.bindValue(":created_at_utc",
                QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch());;
    q.bindValue(":download_sha512", meta->downloadSha512());
    q.bindValue(":title", meta->title());
    q.bindValue(":size", meta->binaryFilesize());
    q.bindValue(":icon_url", meta->iconUrl());
    q.bindValue(":download_url", meta->downloadUrl());
    q.bindValue(":changelog", meta->changelog());
    q.bindValue(":command", meta->command().join(" "));
    q.bindValue(":token", meta->clickToken());
    q.bindValue(":local_version", meta->localVersion());
    q.bindValue(":remote_version", meta->remoteVersion());
    q.bindValue(":kind", UpdateStore::KIND_CLICK);
    q.bindValue(":update_state", updateStateToString(SystemUpdate::UpdateState::StateAvailable));
    q.bindValue(":automatic", meta->automatic());

    if (!q.exec()) {
        qCritical() << "Could not add click update" << q.lastError().text();
    }

    Q_EMIT (changed());
}

ClickUpdateMetadata* UpdateStore::getPending(const QString &id)
{
    ClickUpdateMetadata *m = new ClickUpdateMetadata(this);
    if (!openDb()) return m;

    QSqlQuery q(m_db);
    q.prepare("SELECT id, revision, download_sha512, title, size, icon_url,"
              "download_url, changelog, command, token,"
              "local_version, remote_version, update_state, automatic "
              "FROM updates WHERE id=:id AND state=:state AND kind=:kind");
    q.bindValue(":id", id);
    q.bindValue(":state", UpdateStore::STATE_PENDING);
    q.bindValue(":kind", UpdateStore::KIND_CLICK);

    if (!q.exec()) {
        qCritical() << "Could not fetch click update" << q.lastError().text();
        return m;
    }

    q.next();

    m->setName(q.value(0).toString());
    m->setRevision(q.value(1).toInt());
    m->setDownloadSha512(q.value(2).toString());
    m->setTitle(q.value(3).toString());
    m->setBinaryFilesize(q.value(4).toInt());
    m->setIconUrl(q.value(5).toString());
    m->setDownloadUrl(q.value(6).toString());
    m->setChangelog(q.value(7).toString());
    m->setCommand(q.value(8).toStringList());
    m->setClickToken(q.value(0).toString());
    m->setLocalVersion(q.value(10).toString());
    m->setRemoteVersion(q.value(11).toString());
    m->setAutomatic(q.value(12).toBool());

    q.finish();

    return m;
}

void UpdateStore::add(const QString &kind, const QString &id,
                      const int &revision, const QString &version,
                      const QString &changelog, const QString &title,
                      const QString &iconUrl, const int &size,
                      const bool automatic)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("INSERT OR REPLACE INTO updates (id, revision, state,"
              "created_at_utc, title, size, icon_url, changelog,"
              "remote_version, kind, automatic) VALUES (:id,"
              ":revision, :state, :created_at_utc, :title,"
              ":size, :icon_url, :changelog, :remote_version, :kind, :automatic)");
    q.bindValue(":id", id);
    q.bindValue(":revision", revision);
    q.bindValue(":state", UpdateStore::STATE_PENDING);
    q.bindValue(":created_at_utc",
                QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch());;
    q.bindValue(":title", title);
    q.bindValue(":size", size);
    q.bindValue(":icon_url", iconUrl);
    q.bindValue(":changelog", changelog);
    q.bindValue(":remote_version", version);
    q.bindValue(":kind", kind);
    q.bindValue(":automatic", automatic);

    if (!q.exec()) {
        qCritical() << "Could not add update" << q.lastError().text();
    }

    q.finish();

    Q_EMIT (changed());
}

QSqlDatabase UpdateStore::db() const
{
    return m_db;
}

void UpdateStore::markInstalled(const QString &id, const int &revision)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET state=:state, updated_at_utc=:updated_at_utc"
              " WHERE id=:id AND revision=:revision");
    q.bindValue(":state", UpdateStore::STATE_INSTALLED);
    q.bindValue(":updated_at_utc",
                QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch());
    q.bindValue(":id", id);
    q.bindValue(":revision", revision);

    if (!q.exec()) {
        qCritical() << "could not mark app" << id
                    << "as installed" << q.lastError().text();
    }

    q.finish();

    Q_EMIT (itemChanged(id, revision));
    Q_EMIT (changed());
}

void UpdateStore::setUpdateState(const QString &id, const int &revision,
                                 const SystemUpdate::UpdateState &state)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET update_state=:state"
              " WHERE id=:id AND revision=:revision");
    q.bindValue(":state", updateStateToString(state));
    q.bindValue(":id", id);
    q.bindValue(":revision", revision);
    if (!q.exec()) {
        qCritical() << "could not change state on " << id
                    << "as installed" << q.lastError().text();
    }

    q.finish();

    Q_EMIT (itemChanged(id, revision));
}

void UpdateStore::setProgress(const QString &id, const int &revision,
                              const int &progress)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET progress=:progress"
              " WHERE id=:id AND revision=:revision");
    q.bindValue(":progress", progress);
    q.bindValue(":id", id);
    q.bindValue(":revision", revision);

    if (!q.exec()) {
        qCritical() << "could not set progress on " << id
                    << "as installed" << q.lastError().text();
    }

    q.finish();

    Q_EMIT (itemChanged(id, revision));
}

void UpdateStore::setDownloadId(const QString &id, const int &revision,
                                const QString &downloadId)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET download_id=:did"
              " WHERE id=:id AND revision=:revision");
    q.bindValue(":did", downloadId);
    q.bindValue(":id", id);
    q.bindValue(":revision", revision);

    if (!q.exec()) {
        qCritical() << "could not set download id on " << id
            << q.lastError().text();
    }

    q.finish();

    Q_EMIT (itemChanged(id, revision));
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
                "id TEXT NOT NULL,"
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
                "token TEXT DEFAULT '',"
                "update_state TEXT DEFAULT 'unknown',"
                "progress INTEGER,"
                "automatic INTEGER DEFAULT 0,"
                "download_id TEXT,"
                "PRIMARY KEY (id, revision))");

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

    if (!q.exec()) {
        qCritical() << "could not prune db" << q.lastError().text();
    }
    q.finish();
}

QDateTime UpdateStore::lastCheckDate()
{
    QDateTime d;
    if (!openDb()) return d;

    QSqlQuery q(m_db);
    q.exec("SELECT checked_at_utc FROM meta ORDER BY checked_at_utc DESC;");

    if (q.next()) {
        d = QDateTime::fromMSecsSinceEpoch(q.value(0).toLongLong());
    }
    q.finish();

    return d.toUTC();
}

void UpdateStore::setLastCheckDate(const QDateTime &lastCheckUtc)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("REPLACE INTO meta (checked_at_utc) VALUES (:checked_at_utc)");
    q.bindValue(":checked_at_utc", lastCheckUtc.toMSecsSinceEpoch());

    if (!q.exec()) {
        qCritical() << "could not update checked at value" << q.lastError().text();
    }

    q.finish();
}

QString UpdateStore::updateStateToString(const SystemUpdate::UpdateState &state)
{
    switch (state) {
    case SystemUpdate::UpdateState::StateUnknown:
        return QLatin1String("unknown");
    case SystemUpdate::UpdateState::StateAvailable:
        return QLatin1String("available");
    case SystemUpdate::UpdateState::StateUnavailable:
        return QLatin1String("unavailable");
    case SystemUpdate::UpdateState::StateQueuedForDownload:
        return QLatin1String("queuedfordownload");
    case SystemUpdate::UpdateState::StateDownloading:
        return QLatin1String("downloading");
    case SystemUpdate::UpdateState::StateDownloadingAutomatically:
        return QLatin1String("downloadingautomatically");
    case SystemUpdate::UpdateState::StateDownloadPaused:
        return QLatin1String("downloadpaused");
    case SystemUpdate::UpdateState::StateAutomaticDownloadPaused:
        return QLatin1String("automaticdownloadpaused");
    case SystemUpdate::UpdateState::StateInstalling:
        return QLatin1String("installing");
    case SystemUpdate::UpdateState::StateInstallingAutomatically:
        return QLatin1String("installingautomatically");
    case SystemUpdate::UpdateState::StateInstallPaused:
        return QLatin1String("installpaused");
    case SystemUpdate::UpdateState::StateInstallFinished:
        return QLatin1String("installfinished");
    case SystemUpdate::UpdateState::StateInstalled:
        return QLatin1String("installed");
    case SystemUpdate::UpdateState::StateDownloaded:
        return QLatin1String("downloaded");
    case SystemUpdate::UpdateState::StateFailed:
        return QLatin1String("failed");
    }
    return QLatin1String("unknown");
}

SystemUpdate::UpdateState UpdateStore::stringToUpdateState(const QString &state)
{
    if (state == QLatin1String("available"))
        return SystemUpdate::UpdateState::StateAvailable;
    if (state == QLatin1String("unavailable"))
        return SystemUpdate::UpdateState::StateUnavailable;
    if (state == QLatin1String("queuedfordownload"))
        return SystemUpdate::UpdateState::StateQueuedForDownload;
    if (state == QLatin1String("downloading"))
        return SystemUpdate::UpdateState::StateDownloading;
    if (state == QLatin1String("downloadingautomatically"))
        return SystemUpdate::UpdateState::StateDownloadingAutomatically;
    if (state == QLatin1String("downloadpaused"))
        return SystemUpdate::UpdateState::StateDownloadPaused;
    if (state == QLatin1String("automaticdownloadpaused"))
        return SystemUpdate::UpdateState::StateAutomaticDownloadPaused;
    if (state == QLatin1String("installing"))
        return SystemUpdate::UpdateState::StateInstalling;
    if (state == QLatin1String("installingautomatically"))
        return SystemUpdate::UpdateState::StateInstallingAutomatically;
    if (state == QLatin1String("installpaused"))
        return SystemUpdate::UpdateState::StateInstallPaused;
    if (state == QLatin1String("installfinished"))
        return SystemUpdate::UpdateState::StateInstallFinished;
    if (state == QLatin1String("installed"))
        return SystemUpdate::UpdateState::StateInstalled;
    if (state == QLatin1String("downloaded"))
        return SystemUpdate::UpdateState::StateDownloaded;
    if (state == QLatin1String("failed"))
        return SystemUpdate::UpdateState::StateFailed;
    else
        return SystemUpdate::UpdateState::StateUnknown;
}
} // UpdatePlugin
