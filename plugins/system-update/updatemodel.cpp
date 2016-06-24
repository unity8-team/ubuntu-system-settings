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
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>

#include "systemupdate.h"
#include "updatemodel.h"

namespace UpdatePlugin
{
const QString ALL = "kind, id, local_version, remote_version, revision, \
    state, created_at_utc, updated_at_utc, title, download_sha512, size, \
    icon_url, download_url, command, changelog, click_token, \
    update_state, progress, automatic";

const QString GET_SINGLE = "SELECT " + ALL + " FROM updates WHERE id=:id \
    AND revision=:revision";

const QString GET_ALL = "SELECT " + ALL + " FROM updates";

const QString GET_PENDING_CLICKS = "SELECT " + ALL + ", MAX(revision) FROM \
    updates WHERE state=:state AND kind=:kind GROUP BY id ORDER BY \
    title ASC";

const QString GET_INSTALLED_CLICKS = "SELECT " + ALL + " FROM updates \
    WHERE state=:state AND kind=:kind ORDER BY updated_at_utc DESC";

const QString GET_INSTALLED_SYSUPDATES = "SELECT " + ALL + " FROM updates \
    WHERE state=:state AND kind=:kind ORDER BY updated_at_utc DESC";

const QString GET_INSTALLED = "SELECT " + ALL + " FROM updates WHERE \
    state=:state ORDER BY updated_at_utc";

UpdateModel::UpdateModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_filter(UpdateTypes::All)
    , m_store(new UpdateStore(this))
    , m_updates()
{
    initialize();
}

UpdateModel::UpdateModel(const QString &dbpath, QObject *parent)
    : QAbstractListModel(parent)
    , m_filter(UpdateTypes::All)
    , m_store(new UpdateStore(dbpath, this))
    , m_updates()
{
    initialize();
}

void UpdateModel::initialize()
{
    connect(this, SIGNAL(filterChanged()), SLOT(update()));
    connect(SystemUpdate::instance(), &SystemUpdate::storeChanged,
            this, &UpdateModel::update);
    connect(SystemUpdate::instance(), SIGNAL(storeItemChanged(QString, int)),
            this, SLOT(updateItem(QString, int)));
}

UpdateModel::~UpdateModel()
{
    // query().finish();
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
        names[StateRole] = "state";
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
    }

    return names;
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
    case StateRole:
        return u.state;
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
    }
}

int UpdateModel::rowCount(const QModelIndex &parent) const
{
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

void UpdateModel::update()
{
    qWarning() << "blast update...";
    int oldCount = count();
    if (!m_store->openDb()) return;

    QSqlQuery q(m_store->db());
    switch (m_filter) {
    case UpdateTypes::All:
        q.prepare(GET_ALL);
        break;
    case UpdateTypes::Pending:
    case UpdateTypes::PendingClicksUpdates:
        q.prepare(GET_PENDING_CLICKS);
        q.bindValue(":state", UpdateStore::STATE_PENDING);
        q.bindValue(":kind", UpdateStore::KIND_CLICK);
        break;
    case UpdateTypes::PendingSystemUpdates:
        return; // We don't store these, so bail.
    case UpdateTypes::InstalledClicksUpdates:
        q.prepare(GET_INSTALLED_CLICKS);
        q.bindValue(":state", UpdateStore::STATE_INSTALLED);
        q.bindValue(":kind", UpdateStore::KIND_CLICK);
        break;
    case UpdateTypes::InstalledSystemUpdates:
        q.prepare(GET_INSTALLED_SYSUPDATES);
        q.bindValue(":state", UpdateStore::STATE_INSTALLED);
        q.bindValue(":kind", UpdateStore::KIND_SYSTEM);
        break;
    case UpdateTypes::Installed:
        q.prepare(GET_INSTALLED);
        q.bindValue(":state", UpdateStore::STATE_INSTALLED);
        break;
    }

    if (!q.exec()) {
        qCritical() << "could not update" << q.lastError().text();
        return;
    }

    m_updates.clear();
    beginResetModel();

    while (q.next()) {
        UpdateStruct u;
        setValues(&u, &q);
        m_updates.append(u);
    }

    endResetModel();

    if (oldCount != count()) {
        Q_EMIT (countChanged());
    }
}

void UpdateModel::updateItem(const QString &id, const int &revision)
{
    int idx = find(id, revision);

    if (idx < 0) {
        return;
    }

    if (!m_store->openDb()) return;
    QSqlQuery q(m_store->db());
    q.prepare(GET_SINGLE);
    q.bindValue(":id", id);
    q.bindValue(":revision", revision);

    if (!q.exec()) {
        qCritical() << "could fetch item from db" << q.lastError().text();
        return;
    }

    q.next();

    UpdateStruct u = m_updates.at(idx);
    setValues(&u, &q);
    m_updates.replace(idx, u);

    QModelIndex first = index(idx, 0);
    QModelIndex last = index(first.row(), 0);

    Q_EMIT (dataChanged(first, last));

    // qWarning() << q.value(1).toString();
    // qWarning() << "updateItem" << rowCount() << id << revision;
    // for (int i = 0; i < rowCount(); i++) {
    //     QModelIndex idIdx = index(i, 1);
    //     QString recordId = QSqlQueryModel::data(index(i, 1)).toString();
    //     int recordRev = QSqlQueryModel::data(index(i, 4)).toInt();
    //     QModelIndex last = index(i, columnCount() - 1);
    //     if (recordId == id && recordRev == revision) {
    //         qWarning() << record(i);
    //         record(i).setValue(17, 50);
    //         Q_EMIT (dataChanged(idIdx, last));
    //         break;
    //     }
    // }
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

void UpdateModel::setValues(UpdateStruct *update, QSqlQuery *query)
{
    update->kind = query->value(0).toString();
    update->id = query->value(1).toString();
    update->localVersion = query->value(2).toString();
    update->remoteVersion = query->value(3).toString();
    update->revision = query->value(4).toInt();
    update->state = query->value(5).toString();
    update->createdAt = QDateTime::fromMSecsSinceEpoch(query->value(6).toLongLong());
    update->updatedAt = QDateTime::fromMSecsSinceEpoch(query->value(7).toLongLong());
    update->title = query->value(8).toString();
    update->downloadHash = query->value(9).toString();
    update->size = query->value(10).toInt();
    update->iconUrl = query->value(11).toString();
    update->downloadUrl = query->value(12).toString();
    update->command = query->value("command").toStringList();
    update->changelog = query->value(14).toString();
    update->token = query->value(15).toString();
    update->updateState = m_store->stringToUpdateState(query->value(16).toString());
    update->progress = query->value(17).toInt();
    update->automatic = query->value(18).toBool();
}
} // UpdatePlugin
