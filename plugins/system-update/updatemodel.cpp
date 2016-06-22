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

#include "systemupdate.h"
#include "updatemodel.h"

namespace UpdatePlugin
{

UpdateModel::UpdateModel(QObject *parent)
    : QSqlQueryModel(parent)
    , m_filter(UpdateTypes::All)
    , m_store(new UpdateStore(this))
{
    initialize();
}

UpdateModel::UpdateModel(const QString &dbpath, QObject *parent)
    : QSqlQueryModel(parent)
    , m_filter(UpdateTypes::All)
    , m_store(new UpdateStore(dbpath, this))
{
    initialize();
}

void UpdateModel::initialize()
{
    connect(this, SIGNAL(filterChanged()), SLOT(update()));
    connect(SystemUpdate::instance(), &SystemUpdate::storeChanged,
            this, &UpdateModel::update);
}

UpdateModel::~UpdateModel()
{
    query().finish();
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
    }

    return names;
}

QVariant UpdateModel::data(const QModelIndex &index, int role) const
{
    int col;

    // Note: This is tied to the UpdateStore DB schema.
    switch (role) {
    case Qt::DisplayRole:
    case TitleRole:
        col = 8;
        break;
    case KindRole:
        col = 0;
        break;
    case IconUrlRole:
        col = 11;
        break;
    case IdRole:
        col = 1;
        break;
    case LocalVersionRole:
        col = 2;
        break;
    case RemoteVersionRole:
        col = 3;
        break;
    case RevisionRole:
        col = 4;
        break;
    case StateRole:
        col = 5;
        break;
    case CreatedAtRole:
        col = 6;
        break;
    case UpdatedAtRole:
        col = 7;
        break;
    case DownloadHashRole:
        col = 9;
        break;
    case SizeRole:
        col = 10;
        break;
    case DownloadUrlRole:
        col = 12;
        break;
    case CommandRole:
        col = 13;
        break;
    case ChangelogRole:
        col = 14;
        break;
    case TokenRole:
        col = 15;
        break;
    }

    QModelIndex idx = this->index(index.row(), col);
    // We ask for display role, because we've already determined
    // what data to return.
    return QSqlQueryModel::data(idx, Qt::DisplayRole);
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
    int oldCount = count();
    qWarning() << "update count(old)" << oldCount;
    if (!m_store->openDb()) return;

    QString sql;
    switch (m_filter) {
    case UpdateTypes::All:
        sql = "SELECT * FROM updates";
        break;
    case UpdateTypes::Pending:
    case UpdateTypes::PendingClicksUpdates:
        // FIXME: use prepared statement.
        sql = "SELECT *, MAX(revision) FROM updates WHERE state='"
              + UpdateStore::STATE_PENDING + "' AND kind='"
              + UpdateStore::KIND_CLICK + "' GROUP BY app_id"
              " ORDER BY title ASC";
        break;
    case UpdateTypes::PendingSystemUpdates:
        sql = ""; // We don't store it, use SI instead.
        break;
    case UpdateTypes::InstalledClicksUpdates:
        // FIXME: use prepared statement.
        sql = "SELECT * FROM updates WHERE state='" + UpdateStore::STATE_INSTALLED + "'"
              " AND kind='" + m_store->KIND_CLICK + "' ORDER BY updated_at_utc DESC";
        break;
    case UpdateTypes::InstalledSystemUpdates:
        // FIXME: use prepared statement.
        sql = "SELECT * FROM updates WHERE state='" + UpdateStore::STATE_INSTALLED + "'"
              " AND kind='" + m_store->KIND_SYSTEM + "' ORDER BY updated_at_utc DESC";
        break;
    case UpdateTypes::Installed:
        sql = "SELECT * FROM updates WHERE state='" + UpdateStore::STATE_INSTALLED + "'";
        break;
    }

    if (sql.isEmpty()) {
        return; // nothing to execute.
    }

    setQuery(sql, m_store->db());

    if (lastError().isValid()) {
        qWarning() << lastError();
    }

    if (!query().isActive()) {
        qWarning() << "Query was not active.";
    }

    if (oldCount != count()) {
        Q_EMIT (countChanged());
    }
}
} // UpdatePlugin
