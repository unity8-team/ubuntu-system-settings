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
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QStandardPaths>

#include "updatemodel.h"

namespace UpdatePlugin
{
const QString ALL = "kind, id, local_version, remote_version, revision, \
    installed, created_at_utc, updated_at_utc, title, download_sha512, size, \
    icon_url, download_url, command, changelog, token, \
    update_state, progress, automatic, download_id, error";

const QString GET_SINGLE = "SELECT " + ALL + " FROM updates WHERE id=:id \
    AND revision=:revision";

const QString GET_ALL = "SELECT " + ALL + " FROM updates";

const QString GET_PENDING_CLICKS = "SELECT " + ALL + ", MAX(revision) FROM \
    updates WHERE installed=:installed AND kind=:kind GROUP BY id ORDER BY \
    title ASC";

const QString GET_INSTALLED_CLICKS = "SELECT " + ALL + " FROM updates \
    WHERE installed=:installed AND kind=:kind ORDER BY updated_at_utc DESC";

const QString GET_INSTALLED_SYSUPDATES = "SELECT " + ALL + " FROM updates \
    WHERE installed=:installed AND kind=:kind ORDER BY updated_at_utc DESC";

const QString GET_INSTALLED = "SELECT " + ALL + " FROM updates WHERE \
    installed=:installed ORDER BY updated_at_utc";

const QString UpdateModel::KIND_CLICK = QLatin1String("click");
const QString UpdateModel::KIND_SYSTEM = QLatin1String("system");

UpdateModel::UpdateModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_dbpath("")
    , m_filter(UpdateTypes::All)
    , m_updates()
{
    QString dataPath = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);
    if (Q_UNLIKELY(!QDir().mkpath(dataPath))) {
        qCritical() << "Could not create" << dataPath;
        return;
    }
    m_dbpath = dataPath + QLatin1String("/updatestore.db");

    initializeDb();
    initialize();
}

UpdateModel::UpdateModel(const QString &dbpath, QObject *parent)
    : QAbstractListModel(parent)
    , m_dbpath(dbpath)
    , m_filter(UpdateTypes::All)
    , m_updates()
{
    initializeDb();
    initialize();
}

void UpdateModel::initialize()
{
    connect(this, SIGNAL(filterChanged()), SLOT(refresh()));
    connect(this, SIGNAL(changed()),
            SystemUpdate::instance(), SLOT(notifyModelChanged()));
    connect(this, SIGNAL(updateChanged(QString, int)),
            SystemUpdate::instance(), SLOT(notifyModelItemChanged(QString, int)));
    connect(this, SIGNAL(updateChanged(QString)),
            SystemUpdate::instance(), SLOT(notifyModelItemChanged(QString)));

    connect(SystemUpdate::instance(), SIGNAL(modelChanged()),
            this, SLOT(refresh()));
    connect(SystemUpdate::instance(), SIGNAL(modelItemChanged(QString, int)),
            this, SLOT(refreshItem(QString, int)));
    connect(SystemUpdate::instance(), SIGNAL(modelItemChanged(QString)),
            this, SLOT(refreshItem(QString)));
}

void UpdateModel::initializeDb()
{
    // Create a unique connection name
    int connI = 0;
    while (m_connectionName.isEmpty()) {
        QString tmpl("update-model-%1");
        if (!QSqlDatabase::contains(tmpl.arg(connI)))
            m_connectionName = tmpl.arg(connI);
        connI++;
    }
    qWarning() << "initializeDb with conn name" << m_connectionName << m_dbpath;

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
}


UpdateModel::~UpdateModel()
{
    m_db.close();
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(m_connectionName);
    qWarning() << "removing database" << m_connectionName;
}

QHash<int, QByteArray> UpdateModel::roleNames() const
{
    static QHash<int,QByteArray> names;

    if (Q_UNLIKELY(names.empty())) {
        names[Qt::DisplayRole] = "displayName";
        names[KindRole] = "kind";
        names[IconUrlRole] = "iconUrl";
        names[IdRole] = "identifier";
        names[LocalVersionRole] = "localVersion";
        names[RemoteVersionRole] = "remoteVersion";
        names[RevisionRole] = "revision";
        names[InstalledRole] = "installed";
        names[CreatedAtRole] = "createdAt";
        names[UpdatedAtRole] = "updatedAt";
        names[TitleRole] = "title";
        names[DownloadHashRole] = "downloadHash";
        names[SizeRole] = "size";
        names[DownloadUrlRole] = "downloadUrl";
        names[ChangelogRole] = "changelog";
        names[CommandRole] = "command";
        names[TokenRole] = "token";
        names[UpdateStateRole] = "updateState";
        names[ProgressRole] = "progress";
        names[AutomaticRole] = "automatic";
        names[DownloadIdRole] = "downloadId";
    }

    return names;
}

QSqlDatabase UpdateModel::db() const
{
    return m_db;
}

QVariant UpdateModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    if (row < 0 || row > (m_updates.length() - 1))
        return QVariant();

    UpdateStruct u = m_updates.at(row);

    switch (role) {
    case Qt::DisplayRole:
    case KindRole:
        return u.kind;
    case IdRole:
        return u.id;
    case LocalVersionRole:
        return u.localVersion;
    case RemoteVersionRole:
        return u.remoteVersion;
    case RevisionRole:
        return u.revision;
    case InstalledRole:
        return u.installed;
    case CreatedAtRole:
        return u.createdAt;
    case UpdatedAtRole:
        return u.updatedAt;
    case TitleRole:
        return u.title;
    case DownloadHashRole:
        return u.downloadHash;
    case SizeRole:
        return u.size;
    case IconUrlRole:
        return u.iconUrl;
    case DownloadUrlRole:
        return u.downloadUrl;
    case CommandRole:
        return u.command;
    case ChangelogRole:
        return u.changelog;
    case TokenRole:
        return u.token;
    case UpdateStateRole:
        return (int) u.updateState;
    case ProgressRole:
        return u.progress;
    case AutomaticRole:
        return u.automatic;
    case DownloadIdRole:
        return u.downloadId;
    case ErrorRole:
        return u.error;
    }
    return QVariant();
}

int UpdateModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_updates.size();
}

int UpdateModel::count() const
{
    return rowCount();
}

void UpdateModel::setFilter(const UpdateModel::UpdateTypes &filter)
{
    if (filter != m_filter) {
        m_filter = filter;
        Q_EMIT (filterChanged());
    }
}

UpdateModel::UpdateTypes UpdateModel::filter() const
{
    return m_filter;
}

void UpdateModel::refresh()
{
    qWarning() << "blast refresh...";
    int oldCount = count();
    if (!openDb())
        qWarning() << "could not open db";

    QSqlQuery q(m_db);
    switch (m_filter) {
    case UpdateTypes::All:
        // q = QSqlQuery(GET_ALL, m_db);
        q.prepare(GET_ALL);
        break;
    case UpdateTypes::Pending:
    case UpdateTypes::PendingClicksUpdates:
        q.prepare(GET_PENDING_CLICKS);
        q.bindValue(":installed", false);
        q.bindValue(":kind", UpdateModel::KIND_CLICK);
        break;
    case UpdateTypes::PendingSystemUpdates:
        return; // We don't store these, so bail.
    case UpdateTypes::InstalledClicksUpdates:
        q.prepare(GET_INSTALLED_CLICKS);
        q.bindValue(":installed", true);
        q.bindValue(":kind", UpdateModel::KIND_CLICK);
        break;
    case UpdateTypes::InstalledSystemUpdates:
        q.prepare(GET_INSTALLED_SYSUPDATES);
        q.bindValue(":installed", true);
        q.bindValue(":kind", UpdateModel::KIND_SYSTEM);
        break;
    case UpdateTypes::Installed:
        q.prepare(GET_INSTALLED);
        q.bindValue(":installed", true);
        break;
    }

    if (!q.exec()) {
        qCritical() << "could not update" << q.lastError().text() << q.executedQuery();
        return;
    }

    m_updates.clear();
    beginResetModel();

    while (q.next()) {
        UpdateStruct u;
        u.setValues(&q);
        m_updates.append(u);
    }

    endResetModel();

    if (oldCount != count()) {
        Q_EMIT (countChanged());
    }
}

void UpdateModel::refreshItem(const QString &id, const int &revision)
{
    // qWarning() << Q_FUNC_INFO << id << revision;
    int idx = find(id, revision);

    if (idx < 0) {
        return;
    }

    // qWarning() << Q_FUNC_INFO << "found something";
    refresh(idx);
}

void UpdateModel::refreshItem(const QString &downloadId)
{
    // qWarning() << Q_FUNC_INFO << downloadId;
    int idx = find(downloadId);

    if (idx < 0) {
        return;
    }

    // qWarning() << Q_FUNC_INFO << "found some id";
    refresh(idx);
}

void UpdateModel::refresh(const int &idx)
{
    UpdateStruct update = m_updates[idx];
    // qWarning() << Q_FUNC_INFO << idx << update.id << update.revision;
    if (!openDb()) return;
    QSqlQuery q(m_db);
    q.prepare(GET_SINGLE);
    q.bindValue(":id", update.id);
    q.bindValue(":revision", update.revision);

    if (!q.exec()) {
        qCritical() << "could fetch item from db" << q.lastError().text();
        return;
    }

    q.next();

    update.setValues(&q);
    m_updates.replace(idx, update);

    QModelIndex first = index(idx, 0);
    QModelIndex last = index(first.row(), 0);

    // qWarning() << Q_FUNC_INFO << "datachanged" << idx << first << last;
    Q_EMIT (dataChanged(first, last));
}

void UpdateModel::add(const Update *update)
{
    // qWarning() << "adding" << update->name();
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("INSERT OR REPLACE INTO updates (id, revision, installed,"
              "created_at_utc, download_sha512, title, size, icon_url,"
              "download_url, changelog, command, token,"
              "local_version, remote_version, kind, update_state, automatic) "
              "VALUES (:id, :revision, :installed, :created_at_utc, "
              ":download_sha512, :title, :size, :icon_url, :download_url,"
              ":changelog, :command, :token, :local_version,"
              ":remote_version, :kind, :update_state, :automatic)");
    q.bindValue(":id", update->name());
    q.bindValue(":revision", update->revision());
    q.bindValue(":installed", false);
    q.bindValue(":created_at_utc",
                QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch());;
    q.bindValue(":download_sha512", update->downloadSha512());
    q.bindValue(":title", update->title());
    q.bindValue(":size", update->binaryFilesize());
    q.bindValue(":icon_url", update->iconUrl());
    q.bindValue(":download_url", update->downloadUrl());
    q.bindValue(":changelog", update->changelog());
    q.bindValue(":command", update->command().join(" "));
    q.bindValue(":token", update->token());
    q.bindValue(":local_version", update->localVersion());
    q.bindValue(":remote_version", update->remoteVersion());
    q.bindValue(":kind", UpdateModel::KIND_CLICK);
    q.bindValue(
        ":update_state",
        updateStateToString(
            SystemUpdate::UpdateState::StateAvailable
        )
    );
    q.bindValue(":automatic", update->automatic());

    if (!q.exec()) {
        qCritical() << "Could not add click update" << q.lastError().text();
    }

    Q_EMIT (changed());
}

ClickUpdate* UpdateModel::getPendingClickUpdate(const QString &id)
{
    ClickUpdate *m = new ClickUpdate();
    if (!openDb()) return m;

    QSqlQuery q(m_db);
    q.prepare("SELECT id, revision, download_sha512, title, size, icon_url,"
              "download_url, changelog, command, token,"
              "local_version, remote_version, update_state, automatic "
              "FROM updates WHERE id=:id AND installed=:installed AND kind=:kind");
    q.bindValue(":id", id);
    q.bindValue(":installed", false);
    q.bindValue(":kind", UpdateModel::KIND_CLICK);

    if (!q.exec()) {
        qCritical() << "Could not fetch update" << q.lastError().text();
        return m;
    }

    q.next();

    m->setName(q.value(0).toString());
    m->setRevision(q.value(1).toInt());
    m->setDownloadSha512(q.value(2).toString());
    m->setTitle(q.value("title").toString());
    m->setBinaryFilesize(q.value(4).toInt());
    m->setIconUrl(q.value("icon_url").toString());
    m->setDownloadUrl(q.value(6).toString());
    m->setChangelog(q.value("changelog").toString());
    m->setCommand(q.value("command").toStringList());
    m->setToken(q.value("token").toString());
    m->setLocalVersion(q.value(10).toString());
    m->setRemoteVersion(q.value(11).toString());
    m->setAutomatic(q.value(12).toBool());

    q.finish();

    return m;
}

void UpdateModel::add(const QString &kind, const QString &id,
                      const int &revision, const QString &version,
                      const QString &changelog, const QString &title,
                      const QString &iconUrl, const int &size,
                      const bool automatic)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("INSERT OR REPLACE INTO updates (id, revision, installed,"
              "created_at_utc, title, size, icon_url, changelog,"
              "remote_version, kind, automatic) VALUES (:id,"
              ":revision, :installed, :created_at_utc, :title,"
              ":size, :icon_url, :changelog, :remote_version, :kind, :automatic)");
    q.bindValue(":id", id);
    q.bindValue(":revision", revision);
    q.bindValue(":installed", false);
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

void UpdateModel::setInstalled(const QString &downloadId)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET installed=:installed, "
              "updated_at_utc=:updated_at_utc WHERE download_id=:did");
    q.bindValue(":installed", true);
    q.bindValue(":updated_at_utc",
                QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch());
    q.bindValue(":did", downloadId);

    if (!q.exec()) {
        qCritical() << "could not mark download" << downloadId
                    << "as installed" << q.lastError().text();
    }

    q.finish();

    setState(downloadId, SystemUpdate::UpdateState::StateInstallFinished);
    unsetDownloadId(downloadId);

    Q_EMIT (changed());
}

void UpdateModel::startUpdate(const QString &downloadId)
{
    setState(downloadId, SystemUpdate::UpdateState::StateQueuedForDownload);
}

void UpdateModel::processUpdate(const QString &downloadId)
{
    setState(downloadId, SystemUpdate::UpdateState::StateInstalling);
}

void UpdateModel::setError(const QString &downloadId, const QString &msg)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET error=:error"
              " WHERE download_id=:did");
    q.bindValue(":error", msg);
    q.bindValue(":did", downloadId);
    if (!q.exec()) {
        qCritical() << "could set error on " << downloadId
                    << q.lastError().text();
    }

    q.finish();

    setState(downloadId, SystemUpdate::UpdateState::StateFailed);
    unsetDownloadId(downloadId);
}

void UpdateModel::setState(const QString &downloadId,
                           const SystemUpdate::UpdateState &state)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET update_state=:state"
              " WHERE download_id=:did");
    q.bindValue(":state", updateStateToString(state));
    q.bindValue(":did", downloadId);
    if (!q.exec()) {
        qCritical() << "could not change state on " << downloadId
                    << q.lastError().text();
    }

    q.finish();

    Q_EMIT (updateChanged(downloadId));
}

void UpdateModel::setProgress(const QString &downloadId,
                              const int &progress)
{
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET progress=:progress, "
              "update_state=:state WHERE download_id=:did");
    q.bindValue(":progress", progress);
    q.bindValue(":state",
                updateStateToString(SystemUpdate::UpdateState::StateDownloading));
    q.bindValue(":did", downloadId);

    if (!q.exec()) {
        qCritical() << "could not set progress on " << downloadId
                    << q.lastError().text();
    }

    q.finish();

    Q_EMIT (updateChanged(downloadId));
}

void UpdateModel::setDownloadId(const QString &id, const int &revision,
                                const QString &downloadId)
{
    // qWarning() << Q_FUNC_INFO << id << revision << downloadId;
    if (!openDb()) return;

    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET download_id=:did, error=''"
              " WHERE id=:id AND revision=:revision");
    q.bindValue(":did", downloadId);
    q.bindValue(":id", id);
    q.bindValue(":revision", revision);

    if (!q.exec()) {
        qCritical() << "could not set download id on " << id << revision
            << q.lastError().text();
    }

    q.finish();

    Q_EMIT (updateChanged(id, revision));
}

void UpdateModel::pauseUpdate(const QString &downloadId)
{
    setState(downloadId, SystemUpdate::UpdateState::StateDownloadPaused);
}

void UpdateModel::resumeUpdate(const QString &downloadId)
{
    setState(downloadId, SystemUpdate::UpdateState::StateDownloading);
}

void UpdateModel::cancelUpdate(const QString &downloadId)
{
    setState(downloadId, SystemUpdate::UpdateState::StateAvailable);
    unsetDownloadId(downloadId);
}


void UpdateModel::unsetDownloadId(const QString &downloadId)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE updates SET download_id=null"
              " WHERE download_id=:did");
    q.bindValue(":did", downloadId);

    if (!q.exec()) {
        qCritical() << "could not unsetset download id on " << downloadId
            << q.lastError().text();
    }
}

bool UpdateModel::createDb()
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
                "installed INTEGER NOT NULL,"
                "created_at_utc BIGINT NOT NULL,"
                "updated_at_utc BIGINT,"
                "title TEXT NOT NULL,"
                "download_sha512 TEXT,"
                "size INTEGER NOT NULL,"
                "icon_url TEXT NOT NULL,"
                "download_url TEXT,"
                "command TEXT,"
                "changelog TEXT,"
                "token TEXT,"
                "update_state TEXT DEFAULT 'unknown',"
                "progress INTEGER,"
                "automatic INTEGER DEFAULT 0,"
                "download_id TEXT,"
                "error TEXT,"
                "PRIMARY KEY (id, revision))");

    if (Q_UNLIKELY(!ok)) {
        m_db.rollback();
        return false;
    }

    return m_db.commit();
}

bool UpdateModel::openDb()
{
    if (m_db.isOpen()) return true;
    if (Q_UNLIKELY(!m_db.open())) {
        qCritical() << "Could not open updates database:" << m_db.lastError();
        return false;
    }
    return true;
}

void UpdateModel::pruneDb()
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

QDateTime UpdateModel::lastCheckDate()
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

void UpdateModel::setLastCheckDate(const QDateTime &lastCheckUtc)
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

QString UpdateModel::updateStateToString(const SystemUpdate::UpdateState &state)
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

SystemUpdate::UpdateState UpdateModel::stringToUpdateState(const QString &state)
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

int UpdateModel::find(const QString &id, const int &revision) const
{
    int res = -1;
    for (int i = 0; i < m_updates.size(); i++) {
        if (m_updates.at(i).id == id && m_updates.at(i).revision == revision) {
            return i;
        }
    }
    return res;
}

int UpdateModel::find(const QString &downloadId) const
{
    int res = -1;
    for (int i = 0; i < m_updates.size(); i++) {
        if (m_updates.at(i).downloadId == downloadId) {
            return i;
        }
    }
    return res;
}

bool UpdateModel::contains(const QString& downloadId) const
{
    for (int i = 0; i < m_updates.size(); i++) {
        if (m_updates.at(i).downloadId == downloadId) {
            return true;
        }
    }
    return false;
}
} // UpdatePlugin
